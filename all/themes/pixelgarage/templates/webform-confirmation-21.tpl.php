<?php

/**
 * @file
 * Customize confirmation screen after successful submission.
 *
 * This file may be renamed "webform-confirmation-[nid].tpl.php" to target a
 * specific webform e-mail on your site. Or you can leave it
 * "webform-confirmation.tpl.php" to affect all webform confirmations on your
 * site.
 *
 * Available variables:
 * - $node: The node object for this webform.
 * - $progressbar: The progress bar 100% filled (if configured). This may not
 *   print out anything if a progress bar is not enabled for this node.
 * - $confirmation_message: The confirmation message input by the webform
 *   author.
 * - $sid: The unique submission ID of this submission.
 * - $url: The URL of the form (or for in-block confirmations, the same page).
 */
$message_parts[0] = array('part' => t('Ein herzliches Dankeschön für '), 'class' => 'pxl-title-light');
$message_parts[1] = array('part' => t('Dein geschätzes Engagement für Kinder in Not. '), 'class' => 'pxl-title');
$message_parts[2] = array('part' => t('Dein Beitrag wird in Kürze aufgeschaltet'), 'class' => 'pxl-title-light');

$back_url= '/';

?>

<div class="webform-confirmation">
  <?php foreach ($message_parts as $msg): ?>
    <span class="confirmation-message <?php print $msg['class']; ?>"><?php print $msg['part']; ?></span>
  <?php endforeach; ?>
</div>

<div class="links">
  <a href="<?php print $back_url; ?>"><?php print t('Zurück zur Startseite') ?></a>
</div>
