<p><?= t('in') ?> <?= api_file_link($constant) ?></p>

<?= $documentation ?>

<?= $code ?>

<?php if (!empty($related_topics)) { ?>
<h3><?= t('Related topics') ?></h3>
<?= $related_topics ?>
<?php } ?>
