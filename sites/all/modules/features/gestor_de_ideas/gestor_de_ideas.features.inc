<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function gestor_de_ideas_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
  elseif ($module == "wysiwyg" && $api == "wysiwyg_defaults") {
    return array("version" => 2);
  }
}

/**
 * Implementation of hook_node_info().
 */
function gestor_de_ideas_node_info() {
  $items = array(
    'idea' => array(
      'name' => t('Idea'),
      'module' => 'features',
      'description' => t('Type of content that describe an idea.'),
      'has_title' => '1',
      'title_label' => t('Título'),
      'has_body' => '1',
      'body_label' => t('Cuerpo'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function gestor_de_ideas_views_api() {
  return array(
    'api' => '2',
  );
}
