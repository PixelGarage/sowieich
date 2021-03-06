<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:22
 */

/**
 * Alters the menu entries.
 *
 * @param $items
 */
function pixelgarage_menu_alter(&$items) {
  // hides local task tabs
  pg_login_menu_alter($items);
}

/* =============================================================================
 *    Proximity alter hooks implementation
 * ========================================================================== */

/**
 * Alters the proximity item load parameter array.
 *
 * The load parameter array defines a specific url parameter for each proximity
 * item. This parameter is added at the end of the request url and must be
 * unique and URL conform. The unique parameter defines, which item content has
 * to be loaded from the server (see next api function).
 *
 * @param int   $container_index
 *  Index of proximity container (if more than one container exists in one
 *  page).
 * @param array $view
 *  The view.
 * @param array $ajax_load_params
 *  Array of ajax load parameters to be altered, one for each proximity item
 *  (row) retrieved by the view. Default is the views row index.
 */
function pixelgarage_proximity_load_params_alter($container_index, $view, &$ajax_load_params) {
  // Return the node id as ajax parameter for each item.
  foreach ($view->result as $id => $item) {
    $ajax_load_params[$id] = $item->nid;
  }
}

/**
 * Implements the item specific content as render array or html string.
 * The input parameter $param contains the unique load parameter of the
 * requested item.
 *
 * @param int    $container_index
 *    Index of proximity container (if more than one container exists in one
 *    page).
 * @param string $param
 *    The item specific load parameter (see also
 *    hook_proximity_ajax_load_params_alter).
 * @param mixed  $render_item
 *    The rendered content to be returned to the client. The $render_item
 *    should be replaced either by a string (rendered html content), a render
 *    array or an integer (error code).
 */
function pixelgarage_proximity_render_item_alter($container_index, $param, &$render_item) {
  // return the render array for the specific node, if available
  if ($node = node_load($param)) {
    $view_mode = 'full';
    if (property_exists($node, 'ds_switch') && !empty($node->ds_switch)) {
      // take an alternate view mode set by the ds switch
      $view_mode = $node->ds_switch;
    }
    $render_item = node_view($node, $view_mode);
  }
}


/* =============================================================================
 *    Post content hooks implementation
 * ========================================================================== */
/**
 * Fill the submission values into a node.
 */
function pixelgarage_webform_submission_presave_alter($node, &$submission) {
  //
  // create post node and save nid in the submission
  $post = new stdClass();
  $post->type = 'post';
  $post->language = LANGUAGE_NONE;
  $post->uid = 1;
  $post->status = 0;  // do not publish, before videos are attached
  $post->comment = 0;
  $post->promote = 0;
  node_object_prepare($post);
  //
  // update submission data on the post and save it
  $post->field_e_mail[LANGUAGE_NONE][0]['email'] = $submission->data[1][0]; // email
  $post->field_your_name[LANGUAGE_NONE][0]['value'] = $submission->data[4][0]; // your name
  $post->field_quote[LANGUAGE_NONE][0]['value'] = substr($submission->data[3][0], 0, 100); // Quote
  //
  // store image or video
  $media = file_load($submission->data[2][0]);
  $is_image = $media->filemime === 'image/jpeg' || $media->filemime === 'image/png';
  if ($is_image) {
    // media is an image, enable post immediately
    $post->field_image[LANGUAGE_NONE][0]['fid'] = $submission->data[2][0]; // fid image
    $post->status = 1;
  }
  else {
    // media is a video
    $post->field_video[LANGUAGE_NONE][0]['fid'] = $submission->data[2][0]; // fid video
    $post->field_video[LANGUAGE_NONE][0]['display'] = 1;  // used to save node successfully
  }

  //
  // add post to current campaign and set campaign dependant post title
  $front_path = variable_get('site_frontpage');
  $parts = explode('/', $front_path);
  $campaign_nid = array_pop($parts);
  $campaign_node = node_load($campaign_nid);
  $title = (isset($campaign_node->field_social_media_title) && !empty($campaign_node->field_social_media_title[LANGUAGE_NONE][0]['value'])) ?
    $campaign_node->field_social_media_title[LANGUAGE_NONE][0]['value'] : '@name erzählt...';
  $post->title = t($title, array('@name' => $submission->data[4][0])); // create node title
  $post->field_campaign[LANGUAGE_NONE][0]['target_id'] = $campaign_nid;
  $post = node_submit($post);
  node_save($post);

  //
  // add related post id to submission
  _set_webform_submission_value($node, $submission, 'post_nid', $post->nid);
  if ($is_image) return;

  //
  // For all videos start video conversion with transloadit.
  // The converted videos are downloaded and made accessible by the assembly notification (webhook)
  $context = array('nid' => $post->nid);
  $files = array($media->uri);
  if (!module_exists('transloadit_api') || !transloadit_api_execute_assembly($files, '', NULL, $context)) {
    drupal_set_message(t('The video transcoding could not be performed. Check Transloadit settings and log messages.'), 'error', FALSE);
  }

}

/**
 * Called during assembly notification callback.
 *
 * @param array $context
 *    The state variables to be altered during the asynchronous assembly
 *    notification.
 * @param array $results
 *    An associative array containing the resulting files of a transloadit
 *    assembly.
 * @param array $data
 *    The complete data array of a transloadit assembly response.
 *
 * @see transloadit_api_execute_assembly()
 */
function pixelgarage_transloadit_api_context_alter(&$context, $results, $data) {
  $nid = isset($context['nid']) ? $context['nid'] : null;
  if ($nid) {
    //
    // load node and set status to 1
    $node = node_load($nid);
    $node->status = 1;

    // get aspect ratio
    $aspect_ratio = (int)$results['mp4'][0]['meta']['width'] / (int)$results['mp4'][0]['meta']['height'];
    $node->field_video_aspect_ratio[LANGUAGE_NONE][0]['value'] = $aspect_ratio;

    // add downloaded video path to node
    $node->field_video_mp4[LANGUAGE_NONE][0]['value'] = $results['mp4'][0]['url'];
    $node->field_video_webm[LANGUAGE_NONE][0]['value'] = $results['webm'][0]['url'];

    // add poster image to node
    $poster_url = $results['poster'][0]['url'];
    $file = (object) array(
      'filename' => basename($poster_url),
      'uri' => $poster_url,
      'filemime' => file_get_mimetype($poster_url),
      'filesize' => filesize($poster_url),
      'uid' => 1,
      'status' => FILE_STATUS_PERMANENT,
      'timestamp' => time()
    );
    $file = file_save($file);
    $node->field_image[LANGUAGE_NONE][0] = (array)$file;

    // save node
    node_save($node);
  }
}
