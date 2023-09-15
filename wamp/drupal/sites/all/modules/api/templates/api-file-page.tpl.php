<?php if (!empty($file->version)) { ?>
<p><?= t('Version') ?> <?= $file->version ?></p>
<?php } ?>

<?= $documentation ?>

<?php if (!empty($constants)) { ?>
<h3><?= t('Constants') ?></h3>
<?= $constants ?>
<?php } ?>
<?php if (!empty($globals)) { ?>
<h3><?= t('Globals') ?></h3>
<?= $globals ?>
<?php } ?>
<?php if (!empty($functions)) { ?>
<h3><?= t('Functions') ?></h3>
<?= $functions ?>
<?php } ?>
