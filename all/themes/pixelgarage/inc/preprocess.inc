<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:23
 */

/**
 * Preprocess variables for the html template.
 */
function pixelgarage_preprocess_html(&$vars) {
  // make sure jQuery UI and effects is loaded for anonymous users
  drupal_add_library('system', 'ui');
  //drupal_add_library('system', 'effects');

}

/**
 * Override or insert variables for the page templates.
 */
function pixelgarage_preprocess_page(&$vars) {
  // hide titles on login forms
  pg_login_preprocess_page($vars);

  //
  // replace logo with svg version
  $vars['logo'] = str_replace(array('.jpg', '.png'), '.svg', $vars['logo']);

  //
  // define campaign page
  $vars['is_campaign_page'] = FALSE;
  if (isset($vars['node'])) {
    $node = $vars['node'];
    $vars['is_campaign_page'] = $node->type == 'campaign';
  }
  if ($vars['is_campaign_page']) {
    //
    // configure shariff buttons
    $has_mail_subject = !empty($node->field_social_mail_subject) && !empty($node->field_social_mail_subject[LANGUAGE_NONE][0]['value']);
    $has_mail_body = !empty($node->field_social_mail_body) && !empty($node->field_social_mail_body[LANGUAGE_NONE][0]['value']);
    $mail_subject = $has_mail_subject ? $node->field_social_mail_subject[LANGUAGE_NONE][0]['value'] : t("#SoWieIch: Kindern in Not helfen...");
    $mail_body = $has_mail_body ? $node->field_social_mail_body[LANGUAGE_NONE][0]['value'] : t('Setz ein Zeichen gegen die Angst von Kindern. Zeige Deine Anteilnahme und lade Deine als Kind erlebte Angst auf unsere Plattform. Zeige mit einem kurzen Video, dass Dir die Angst von Kindern in Not auf der Welt nicht egal ist. Schaue dir https://sowieich.ch an.');
    $shariff_attrs = array(
      'data-services' => '["facebook","twitter","mail"]',
      'data-orientation' => "horizontal",
      'data-mail-url' => "mailto:",
      'data-mail-subject' => $mail_subject,
      'data-mail-body' => $mail_body,
      'data-lang' => "de",
    );
    $vars['shariff_attributes'] = drupal_attributes($shariff_attrs);
      // load library
    $variant = 'naked'; //variable_get('shariff_css', 'complete');
    libraries_load('shariff', $variant);

    //
    // set campaign specific meta tags
    $is_front = $node->field_current_campaign[LANGUAGE_NONE][0]['value'] == '1';
    $metatag_instance = $is_front ? 'global:frontpage' : 'node:campaign';
    $has_descr = !empty($node->field_text) && !empty($node->field_text[LANGUAGE_NONE][0]['value']);
    if ($has_descr) {
      $descr = $node->field_text[LANGUAGE_NONE][0]['value'];
      $vars['page']['content']['metatags'][$metatag_instance]['description']['#attached']['drupal_add_html_head'][0][0]['#value'] = $descr;
      $vars['page']['content']['metatags'][$metatag_instance]['og:description']['#attached']['drupal_add_html_head'][0][0]['#value'] = $descr;
      $vars['page']['content']['metatags'][$metatag_instance]['twitter:description']['#attached']['drupal_add_html_head'][0][0]['#value'] = $descr;
    }
    $has_keywords = !empty($node->field_keywords) && !empty($node->field_keywords[LANGUAGE_NONE][0]['value']);
    if ($has_keywords) {
      $keywords = $node->field_keywords[LANGUAGE_NONE][0]['value'];
      $vars['page']['content']['metatags'][$metatag_instance]['keywords']['#attached']['drupal_add_html_head'][0][0]['#value'] = $keywords;
    }
    $has_img = !empty($node->field_social_media_image) && !empty($node->field_social_media_image[LANGUAGE_NONE][0]['fid']);
    if ($has_img) {
      $img = file_load($node->field_social_media_image[LANGUAGE_NONE][0]['fid']);
      $img_url = file_create_url($img->uri);
      $vars['page']['content']['metatags'][$metatag_instance]['image_src']['#attached']['drupal_add_html_head'][0][0]['#value'] = $img_url;
      $vars['page']['content']['metatags'][$metatag_instance]['og:image']['#attached']['drupal_add_html_head'][0][0]['#value'] = $img_url;
      $vars['page']['content']['metatags'][$metatag_instance]['og:image:url']['#attached']['drupal_add_html_head'][0][0]['#value'] = $img_url;
      $vars['page']['content']['metatags'][$metatag_instance]['og:image:secure_url']['#attached']['drupal_add_html_head'][0][0]['#value'] = $img_url;
      $vars['page']['content']['metatags'][$metatag_instance]['twitter:image']['#attached']['drupal_add_html_head'][0][0]['#value'] = $img_url;
    }

  }
}


/* =============================================================================
 *      Front Masonry
 * ========================================================================== */
/**
 * Adapts the proximity view, so that always two randomly chosen testimonials are visible
 * in the masonry.
 */
function pixelgarage_preprocess_views_view_proximity (&$vars) {
  $view = $vars['view'];
  if ($view->name != 'postings') return;

  // add testimonials for the social masonry
  $active_campaign_nid = $view->args[0];
  $vars['testimonials'] = _get_random_testimonials($active_campaign_nid);
}

/**
 * Gets the testimonials from the DB.
 */
function _get_random_testimonials($active_campaign_nid) {
  //
  // get the published testimonials in given language
  static $testimonials = array();
  if (empty($testimonials)) {
    // get the published testimonials
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'testimonial')
      ->propertyCondition('status', NODE_PUBLISHED)
      ->fieldCondition('field_campaign', 'target_id', $active_campaign_nid);
    $result = $query->execute();
    if ($result && !empty($result['node'])) {
      $testimonials = node_load_multiple(array_keys($result['node']));
    }
  }
  if (count($testimonials) <= 0) return array();

  //
  // render the testimonials and add the link (language dependent)
  $default_link = 'http://www.kindernothilfe.ch';
  $linked_testimonials = array();
  foreach ($testimonials as $key => $testimonial) {
    $testimonial_link = !empty($testimonial->field_link) ? $testimonial->field_link[LANGUAGE_NONE][0]['url'] : $default_link;
    $testimonial_teaser = node_view($testimonial, 'teaser');
    $linked_testimonials[$key] = array (
      'rendered_testimonial' => render($testimonial_teaser),
      'testimonial_link' => $testimonial_link,
      'css_class' => 'testimonial-bg-color-' . mt_rand(0, 6),
    );
  }

  //
  // return a random testimonial list (index 0..6)
  if (count($linked_testimonials) < 2){
    $i1 = mt_rand(0, 6);
    $keys = array_rand($linked_testimonials, 1);
    return array(
      $i1 => $linked_testimonials[$keys],
    );
  }
  else {
    $i1 = mt_rand(0, 6);
    $i2 = mt_rand(0, 6);
    $keys = array_rand($linked_testimonials, 2);
    return array(
      $i1 => $linked_testimonials[$keys[0]],
      $i2 => $linked_testimonials[$keys[1]],
    );
  }
}


