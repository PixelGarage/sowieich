<?php
define('SDK_ROOT', dirname(dirname(__FILE__)));

function findExamples() {
  return glob(SDK_ROOT.'/example/*.php');
}

function parseExample($file) {
  $php = file_get_contents($file);
  preg_match('/\/\*(.+?)\*\/\n(.+)/s', $php, $match);
  list(, $markdown, $code) = $match;

  return join("\n", array(
    $markdown,
    '```php',
    '<?php',
    $code,
    '```'
  ));
}

$examples = array();
$files = findExamples();
foreach ($files as $file) {
  $example = parseExample($file);
  $examples[] = $example;
}

$readmePath = SDK_ROOT.'/README.md';
$readme = file_get_contents($readmePath);
$exampleDocs = join("\n", $examples);

$readme = preg_replace(
  '/(generated by: make docs[^\n]+\n).+(\n<!--)/is',
  '$1'.$exampleDocs.'$2',
  $readme
);

file_put_contents($readmePath, $readme);