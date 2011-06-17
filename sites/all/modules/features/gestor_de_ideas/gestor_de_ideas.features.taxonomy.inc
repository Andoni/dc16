<?php

/**
 * Implementation of hook_taxonomy_default_vocabularies().
 */
function gestor_de_ideas_taxonomy_default_vocabularies() {
  return array(
    'etiqueta' => array(
      'name' => 'Proceso',
      'description' => 'Etiqueta de la idea.',
      'help' => '',
      'relations' => '1',
      'hierarchy' => '1',
      'multiple' => '0',
      'required' => '1',
      'tags' => '0',
      'module' => 'features_etiqueta',
      'weight' => '0',
      'language' => '',
      'nodes' => array(
        'idea' => 'idea',
      ),
    ),
  );
}
