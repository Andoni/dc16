<?php

/**
 * Implementation of hook_menu_default_menu_custom().
 */
function gestor_de_ideas_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-mymenu
  $menus['menu-mymenu'] = array(
    'menu_name' => 'menu-mymenu',
    'title' => 'my menu',
    'description' => 'Menu para evitar desapariciones de links.',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Menu para evitar desapariciones de links.');
  t('my menu');


  return $menus;
}
