<?php
/**
 * @file
 * Bootstrap 12 template for Display Suite.
 */
//
// shariff button definition
libraries_load('shariff', 'naked');
$shariff_attrs = array(
  'data-services' => '["twitter","facebook","mail"]',
  'data-orientation' => "horizontal",
  'data-mail-url' => "mailto:",
  'data-mail-subject' => variable_get('shariff_mail_subject', "SoWieIch Kindern in Not helfen..."),
  'data-mail-body' => variable_get('shariff_mail_subject', "Die Kindernothilfe Schweiz ..."),
  'data-lang' => "de",
);

//
// logo and play button
$logo_path = drupal_get_path('theme', 'pixelgarage') . '/images/';
$logo_url = file_create_url($logo_path . 'knh_logo_rgb_neg.png');
$play_button = file_create_url($logo_path . 'icon-play.svg');
$color_class = 'bg-color-' . mt_rand(0, 6);

//
// get video paths and poster image
$video_poster = '';
$mp4_source_url = '';
$webm_source_url = '';
if (!empty($node->field_image) && $img = file_load($node->field_image[LANGUAGE_NONE][0]['fid'])) {
  $video_poster = file_create_url($img->uri);
}
if (!empty($node->field_video_mp4)) {
  $mp4_source_url = file_create_url($node->field_video_mp4[LANGUAGE_NONE][0]['value']);
}
if (!empty($node->field_video_webm)) {
  $webm_source_url = file_create_url($node->field_video_webm[LANGUAGE_NONE][0]['value']);
}

//
// add aspect-ratio classes
$video_classes = 'hd-video';
if (!empty($node->field_video_aspect_ratio)) {
  $aspect_ratio = $node->field_video_aspect_ratio[LANGUAGE_NONE][0]['value'];
  $video_classes .= ($aspect_ratio > 1) ? ' normal' : ' edge-wise';
}
?>


<<?php print $layout_wrapper; print $layout_attributes; ?> class="<?php print $classes; ?>">
  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>
  <div class="row">
    <<?php print $central_wrapper; ?> class="col-sm-12 <?php print $central_classes; ?>">
    <?php if ($teaser): ?>
      <?php print $central; ?>
      <div class="play-button">
        <img class="logo-hover" src="<?php print $play_button ?>"/>
      </div>
    <?php else: ?>
      <div class="social-buttons">
        <div class="shariff" <?php print drupal_attributes($shariff_attrs); ?>></div>
      </div>
      <div class="video-container <?php print $video_classes; ?>">
        <!-- video frame -->
        <video id="user-video" preload="auto" poster="<?php print $video_poster; ?>">
          <?php if ($mp4_source_url): ?><source src="<?php print $mp4_source_url; ?>" type="video/mp4"><?php endif; ?>
          <?php if ($webm_source_url): ?><source src="<?php print $webm_source_url; ?>" type="video/webm"><?php endif; ?>
          Your browser doesn't support HTML5 video tag.
        </video>
        <!-- video cover -->
        <div class="colored-side <?php print $color_class; ?>">
          <?php print render($content['field_your_name']); ?>
          <?php print render($content['field_quote']); ?>
        </div>
        <div class="play-button">
          <img class="logo-normal" src="<?php print $logo_url ?>"/>
          <img class="logo-hover" src="<?php print $play_button ?>"/>
        </div>
      </div>
    <?php endif; ?>
    </<?php print $central_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
