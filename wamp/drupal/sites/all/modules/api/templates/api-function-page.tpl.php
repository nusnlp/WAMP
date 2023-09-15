<p><?= api_file_link($function) ?>, <?= t('line') ?> <?= $function->start_line ?></p>

<dl id="api-function-signature">
<dt class="header"><?= t('Versions') ?></dt>
<?php foreach ($signatures as $branch => $signature) { ?>
  <?php if ($signature['active']) { ?>
    <dt><strong><?= $branch ?></strong></dt>
    <dd><strong><code><?= $signature['signature'] ?></code></strong></dd>
  <?php } else { ?>
    <dt><?= l($branch, $signature['url']) ?></dt>
    <dd><code><?= l($signature['signature'], $signature['url'], array('html' => TRUE)) ?></code></dd>
  <?php } ?>
<?php } ?>
</dl>

<?= $documentation ?>

<?php if (!empty($parameters)) { ?>
<h3><?= t('Parameters') ?></h3>
<?= $parameters ?>
<?php } ?>

<?php if (!empty($return)) { ?>
<h3><?= t('Return value') ?></h3>
<?= $return ?>
<?php } ?>

<?php if (!empty($related_topics)) { ?>
<h3><?= t('Related topics') ?></h3>
<?= $related_topics ?>
<?php } ?>

<?= $call ?>

<?php if (!empty($code)) { ?>
<h3><?= t('Code') ?></h3>
<?= $code ?>
<?php } ?>
