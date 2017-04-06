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


