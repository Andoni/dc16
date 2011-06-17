<?php

/**
 * Implementation of hook_wysiwyg_defaults().
 */
function gestor_de_ideas_wysiwyg_defaults() {
  $export = array();
  $wysiwyg = new stdClass;
  $wysiwyg->disabled = FALSE; /* Edit this to true to make a default wysiwyg disabled initially */
  $wysiwyg->api_version = 2;
  $wysiwyg->format = 1;
  $wysiwyg->editor = 'tinymce';
  $wysiwyg->settings = array(
    'default' => 1,
    'user_choose' => 0,
    'show_toggle' => 1,
    'theme' => 'advanced',
    'language' => 'en',
    'buttons' => array(
      'default' => array(
        'bold' => 1,
        'italic' => 1,
        'underline' => 1,
        'strikethrough' => 1,
        'justifyleft' => 1,
        'justifycenter' => 1,
        'copy' => 1,
        'paste' => 1,
      ),
      'drupal' => array(
        'imgupload' => 1,
      ),
    ),
    'toolbar_loc' => 'top',
    'toolbar_align' => 'left',
    'path_loc' => 'bottom',
    'resizing' => 1,
    'verify_html' => 1,
    'preformatted' => 0,
    'convert_fonts_to_spans' => 1,
    'remove_linebreaks' => 1,
    'apply_source_formatting' => 0,
    'paste_auto_cleanup_on_paste' => 0,
    'block_formats' => 'p,address,pre,h2,h3,h4,h5,h6,div',
    'css_setting' => 'theme',
    'css_path' => '',
    'css_classes' => '',
  );

  $export[$wysiwyg->format] = $wysiwyg;
  return $export;  $wysiwyg = new stdClass;
  $wysiwyg->disabled = FALSE; /* Edit this to true to make a default wysiwyg disabled initially */
  $wysiwyg->api_version = 2;
  $wysiwyg->format = 2;
  $wysiwyg->editor = 'tinymce';
  $wysiwyg->settings = array(
    'default' => 1,
    'user_choose' => 0,
    'show_toggle' => 1,
    'theme' => 'advanced',
    'language' => 'en',
    'buttons' => array(
      'default' => array(
        'bold' => 1,
        'italic' => 1,
        'underline' => 1,
        'strikethrough' => 1,
        'justifyleft' => 1,
        'justifycenter' => 1,
        'justifyright' => 1,
        'justifyfull' => 1,
        'bullist' => 1,
        'numlist' => 1,
        'outdent' => 1,
        'indent' => 1,
        'undo' => 1,
        'redo' => 1,
        'link' => 1,
        'unlink' => 1,
        'anchor' => 1,
        'cleanup' => 1,
        'forecolor' => 1,
        'backcolor' => 1,
        'sup' => 1,
        'sub' => 1,
        'blockquote' => 1,
        'code' => 1,
        'hr' => 1,
        'cut' => 1,
        'copy' => 1,
        'paste' => 1,
        'visualaid' => 1,
        'removeformat' => 1,
        'charmap' => 1,
        'help' => 1,
      ),
      'font' => array(
        'fontselect' => 1,
        'fontsizeselect' => 1,
        'styleselect' => 1,
      ),
      'paste' => array(
        'pasteword' => 1,
      ),
      'drupal' => array(
        'imgupload' => 1,
      ),
    ),
    'toolbar_loc' => 'top',
    'toolbar_align' => 'left',
    'path_loc' => 'bottom',
    'resizing' => 1,
    'verify_html' => 1,
    'preformatted' => 0,
    'convert_fonts_to_spans' => 1,
    'remove_linebreaks' => 1,
    'apply_source_formatting' => 0,
    'paste_auto_cleanup_on_paste' => 0,
    'block_formats' => 'p,address,pre,h2,h3,h4,h5,h6,div',
    'css_setting' => 'theme',
    'css_path' => '',
    'css_classes' => '',
  );

  $export[$wysiwyg->format] = $wysiwyg;
  return $export;
}
