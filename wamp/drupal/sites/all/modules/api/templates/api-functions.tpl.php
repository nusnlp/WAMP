<dl class="api-functions">
<?php foreach ($functions as $function) { ?>
  <dt><?= $function['function'] ?> <small>in <?= $function['file'] ?></small></dt>
  <dd><?= $function['description'] ?></dd>
<?php } ?>
</dl>
