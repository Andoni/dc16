<?php

/**
 * Implementation of hook_strongarm().
 */
function gestor_de_ideas_strongarm() {
  $export = array();
  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'content_extra_weights_idea';
  $strongarm->value = array(
    'title' => '-9',
    'body_field' => '-5',
    'revision_information' => '-1',
    'author' => '0',
    'options' => '1',
    'comment_settings' => '5',
    'language' => '3',
    'translation' => '6',
    'menu' => '-4',
    'taxonomy' => '-7',
    'book' => '-2',
    'path' => '4',
    'attachments' => '7',
    'og_nodeapi' => '2',
    'print' => '9',
    'vud_node_widget_display' => '-6',
    'workflow' => '8',
  );

  $export['content_extra_weights_idea'] = $strongarm;
  return $export;
}
