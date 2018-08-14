<?php
/**
 * @file
 * Bootstrap 12 template for Display Suite.
 */
$header_has_image = !empty($node->field_image) && !empty($node->field_image[LANGUAGE_NONE][0]['fid']);

// get post view
$postings = views_embed_view('postings', 'block', $node->nid);

?>


<<?php print $layout_wrapper; print $layout_attributes; ?> class="<?php print $classes; ?>">
  <?php if (isset($title_suffix['contextual_links'])): ?>
    <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>
  <div class="row">
    <<?php print $central_wrapper; ?> class="col-sm-12 <?php print $central_classes; ?>">
      <?php if ($header_has_image): ?>
        <?php print render($content['field_image']); ?>
      <?php else: ?>
        <?php print render($content['body']); ?>
      <?php endif; ?>
      <?php print $postings; ?>
    </<?php print $central_wrapper; ?>>
  </div>
</<?php print $layout_wrapper ?>>


<!-- Needed to activate display suite support on forms -->
<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
