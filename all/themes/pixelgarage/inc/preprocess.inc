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
  // define shariff buttons
  if ($vars['is_front']) {
    $shariff_attrs = array(
      'data-services' => '["twitter","facebook","mail"]',
      'data-orientation' => "horizontal",
      'data-mail-url' => "mailto:",
      'data-mail-subject' => variable_get('shariff_mail_subject', "SoWieIch Kindern in Not helfen..."),
      'data-mail-body' => variable_get('shariff_mail_subject', "Die Kindernothilfe Schweiz ..."),
      'data-lang' => "de",
      'data-url' => "http://sowieich.ch",
    );
    $vars['shariff_attributes'] = drupal_attributes($shariff_attrs);

    // load library
    $variant = 'naked'; //variable_get('shariff_css', 'complete');
    libraries_load('shariff', $variant);
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
  $vars['testimonials'] = _get_random_testimonials();
}

/**
 * Gets the testimonials from the DB.
 */
function _get_random_testimonials() {
  //
  // get the published testimonials in given language
  static $testimonials = array();
  if (empty($testimonials)) {
    // get the published testimonials
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'testimonial')
      ->propertyCondition('status', NODE_PUBLISHED);
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


