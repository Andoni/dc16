<?php
// $Id: ponderator.admin.inc, v0.3.001 2011/02/07

module_load_include('inc', 'ponderator', 'includes/ponderator.db');

function ponderator_admin_form() {
	$ntNames = array();
	$nodetypes		= node_get_types();
	foreach($nodetypes as $nodetype)
		$ntNames[$nodetype->type] = $nodetype->name;
	
	$vNames['none'] = 'NONE';
	$vocabularies	= taxonomy_get_vocabularies();
	foreach($vocabularies as $vocabulary)
		$vNames[$vocabulary->vid] = $vocabulary->name;
	
	
	$form['admin'] = array(
    	'#type'		=> 'fieldset',
    	'#title'	=> t('Administer ponderator'),
    	//'#suffix'	=> l(t('Configure processes'),'admin/settings/ponderator/config'),
	);
	
	$form['admin']['nodetype'] = array(
		'#type' => 'select',
		'#title' => t('Node Type'),
		'#default_value' => variable_get('ponderator_nodetype','0'),
		'#options' => $ntNames,
		'#description' => t('Node type to rate.'),
	);
	$form['admin']['vid'] = array(
		'#type' => 'select',
		'#title' => t('Processes vocabulary name'),
		'#default_value' => variable_get('ponderator_vid','0'),
		'#options' => $vNames,
		'#description' => t('Select the processes vocabulary name. "NONE" will make a simple process without name'),
	);
    
    $form['admin']['submit'] = array(
		'#title'	=> t('Save'),
		'#type'		=> 'submit',
		'#value'	=> t('Save'),
    );

  return $form;
}

function ponderator_admin_form_submit($form, &$form_state) {
	variable_set("ponderator_nodetype",	$form_state['values']['nodetype']);
	variable_set("ponderator_vid",		$form_state['values']['vid']);
	drupal_set_message(t("Configuration saved"));
}
	