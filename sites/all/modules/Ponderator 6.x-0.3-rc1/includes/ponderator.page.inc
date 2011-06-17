<?php
// $Id: ponderator.page.inc, v0.2.002 2011/02/02

module_load_include('inc', 'ponderator', 'includes/ponderator.db');
/**
 * List of process with the link to the ponderation form
 */
function ponderator_list(){
	$header = array(
		t("id"),
		t("Process"),
		t("Rate"),
	);
	
	$processes = taxonomy_get_tree(variable_get("ponderator_vid","0"));
	foreach($processes as $process){
		if(user_access("access ponderator ".$process->tid)){
			$options['attributes']['title']	=	t('Rate process: ').$process->name;
			$rows[] = array(
				$process->tid,
				$process->name,
				l('rate','ponderator/'.$process->tid,$options),
			);
		}
	}
	
	$ret =  theme('table',$header,$rows);
	return $ret;
}

/**
 * Form that displays the textfields with the current ponderation values and the totals
 * 
 * @param $form_state
 * @param $tid
 * 	Process identifier
 */
function ponderator_all_form(&$form_state,$tid){
	global $user;
	$criteria		=	ponderator_get_criteria($tid);
	$criteriacount	=	count($criteria);
	$nodes			=	ponderator_get_nodes($tid);
	$nodecount		=	count($nodes);
	$ratings		=	ponderator_get_ratings($user->uid, $tid);
	
	if($nodecount)
		foreach($nodes as $node){
			$rating_sum = 0;	//Saves the sum of ratings for this node 
			if($criteriacount>0)
				foreach($criteria as $criterion){
					if(isset($ratings[$node['nid']][$criterion['cid']])){//previously rated
						$default_value = $ratings[$node['nid']][$criterion['cid']];
						$rating_sum	+=	$ratings[$node['nid']][$criterion['cid']]*$criterion['weight'];
					}else{//Not rated yet
						$default_value = '';
					}
					$form['matrix'][$node['nid']][$criterion['cid']] = array(
						'#type'				=>	'textfield',
						'#size'				=>	'1',
						'#default_value'	=>	$default_value,
						'#parents'			=>	array('matrix',$node['nid'],$criterion['cid']),
					);
				}
			$form['matrix'][$node['nid']]['total'] = array(
				'#type'				=>	'textfield',
				'#size'				=>	'1',
				'#default_value'	=>	$rating_sum,
				'#disabled'			=>	true,
				'#parents'			=>	array('matrix',$node['nid'],'total'),
			);
		}
	else
		drupal_set_message(t('No nodes for this process'),'warning');
	$form['submit']= array(
		'#title'	=>	t('Save'),
		'#type'		=>	'submit',
		'#value'	=>	t('Save'),
	);
	
	/*to call the "theme_ponderator_all_form" function,
	passes throught "ponderator_theme" function*/
	$form['#theme'] = 'ponderator_all_form';
	
	return $form;
}

function theme_ponderator_all_form($form){
	$tid			=	arg(1);
	$term 			=	taxonomy_get_term($tid);
	$criteria		=	ponderator_get_criteria($tid);
	$criteriacount	=	count($criteria);
	$nodes			=	ponderator_get_nodes($tid);
	$nodecount		=	count($nodes);
	$mode			=	ponderator_get_mode($tid);
	
	//Before the table
	$ret	 = '<h2>'.t($term->name.' process').'</h2>';
	
	//Help table
	$h		 = array();
	$rs[]	 = array(
				'<b>'.t('Mode').'</b>',
				$mode['name']
			   );
	$rs[]	 = array(
				'<b>'.t('Posible values').'</b>',
				($mode['type']=='SP' && $nodecount!=0) ? '1-'.$nodecount : $mode['value'],
			   );
	$rs[]	 = array(
				'<b>'.t('Description').'</b>',
				$mode['description']
			   );
	
	$ret	.=	'<div style="margin-right:auto;margin-left:auto;width: 50%;">'.
				'<h4>'.t('Ponderation mode').'</h4>'.
				theme_table($h, $rs, array('style'=>'text-align: center;')).
				'</div>';
	
	//Header of the table
	$header[] = t('ideia');
	if($criteriacount>0)
		foreach($criteria as $criterion){
			$options['attributes']['title']	=	$criterion['description'];
			$header[] = l(
							$criterion['name'].' ('.$criterion['weight'].')',
							'ponderator/criterion/'.$criterion['cid'],
							$options
						);
		}
	$header[] = t('TOTAL');
	
	//Rows of the table
	if($nodecount>0)
		foreach($nodes as $node){
			$row = array();
			$row[] = l($node['title'],'node/'.$node['nid']);
			if($criteriacount>0)
				foreach($criteria as $criterion)
					$row[] = drupal_render($form['matrix'][$node['nid']][$criterion['cid']]);
			$row[] = drupal_render($form['matrix'][$node['nid']]['total']);
			$rows[]=$row;
		}
	$ret .= theme('table',$header,$rows);
	
	//Any other thing (submit...)
	$ret .= drupal_render($form);
	
	return $ret;
}

/**
 * ponderator_all_form form validation function, not to save ilegal values in the database.
 * @param $form
 * @param $form_state
 */
function ponderator_all_form_validate($form, &$form_state) {
	$tid=arg(1);
	$mode		=	ponderator_get_mode($tid);
	$criteria	=	ponderator_get_criteria($tid);
	$nodes		=	ponderator_get_nodes($tid);
  	switch($mode['type']){
  		case('N')://normal
  			ponderator_normal_validation($form_state,$nodes,$criteria);
  			break;
  		case('SP')://PonderatedSelection
  			ponderator_ps_validation($form_state,$nodes,$criteria);
  			break;
  		case('MC')://CriteriaMatrix
  			ponderator_cm_validation($form_state,$nodes,$criteria,$tid,$mode['value']);
  			break;
  	}
}

/**
 * Only numeric values between 0 and 10
 * @param $form_state
 * 	form state taken from the validation function
 * @param $nodes
 * 	array of ideas
 * @param $process
 * 	string with the process name
 * @param $criterias
 * 	array of strings with criteria names
 */
function ponderator_normal_validation(&$form_state,$nodes,$criteria){
	foreach($nodes as &$node)
		foreach($criteria as $criterion){
			$value		=	$form_state['values']['matrix'][$node['nid']][$criterion['cid']];
			$field_name	=	'matrix]['.$node['nid'].']['.$criterion['cid'];
			if(	$value!=''){
				if(!is_numeric($value))
	          		form_set_error(
	          			$field_name, 
	          			t('You must eneter numeric values.')
	          		);
	        	else if( ($value<0) || ($value>10))
	          		form_set_error(
	          			$field_name, 
	          			t('Values must be between 0 and 10')
	          		);
			}
		}
}

/**
 * Only numeric values between 1 and number of ideas
 * values can not be repeated for the same criteria
 * @param $form_state
 * 	form state taken from the validation function
 * @param $nodes
 * 	array of ideas
 * @param $criteria
 * 	array of criteria
 */
function ponderator_ps_validation(&$form_state,$nodes,$criteria){
	$ideaCount = count($nodes);
	foreach($criteria as $criterion)
		foreach($nodes as $node){
			$value		=	$form_state['values']['matrix'][$node['nid']][$criterion['cid']];
			$field_name	=	'matrix]['.$node['nid'].']['.$criterion['cid'];
			if(	$value!=''){
				if( ($value<=0) || ($value>$ideaCount))
          			form_set_error(
          				$field_name,
          				t('Values must be between 0 and the number of ideas ('.$ideaCount.').')
          			);
          		if(is_string_in_array($value,$values[$criterion['cid']]))
          			form_set_error(
          				$field_name,
          				t('Values can not be repeated for the same criteria.')
          			);
          		$values[$criterion['cid']][] = $value;
			}
		}
}

/**
 * Only numeric values definied in the administration configuration
 * Values can not be repeated for the same criteria
 * @param $form_state
 * 	form state taken from the validation function
 * @param $nodes
 * 	array of ideas
 * @param $process
 * 	string with the process name
 * @param $criterias
 * 	array of strings with criteria names
 * @param $tid
 * 	integer with process identifier
 */
function ponderator_cm_validation(&$form_state,$nodes,$criteria,$tid,$posibleValuesStr){
	$posibleValues = explode(',',$posibleValuesStr);
	$ideaCount = count($nodes);
	foreach($criteria as &$criterion)
		foreach($nodes as &$node){
			$value		=	$form_state['values']['matrix'][$node['nid']][$criterion['cid']];
			$field_name	=	'matrix]['.$node['nid'].']['.$criterion['cid'];
			if(	$value!=''){
				if(!is_string_in_array($value,$posibleValues))
          			form_set_error(
          				$field_name, 
          				t('Values must be in: '.$posibleValuesStr)
          			);
          		elseif(is_string_in_array($value,$values[$criterion['cid']]))
          			form_set_error(
          				$field_name, 
          				t('Values can not be repeated for the same criteria.')
          			);
          		$values[$criterion['cid']][] = $value;
			}
		}
}

/**
 * Function that saves form values in the database
 * 
 * @param $form
 * @param $form_state
 */
function ponderator_all_form_submit($form, &$form_state) {
	global $user;
	$nids	= array_keys($form_state['values']['matrix']);
	$errors	=	false;
	foreach($nids as $nid){
		$cids	= array_keys($form_state['values']['matrix'][$nid]);
		foreach($cids as $cid){
			if($cid!='total'){
				$rate	=	$form_state['values']['matrix'][$nid][$cid];
				if($rate=='-'||$rate=='')
					$rate = 'null';
				if(!ponderator_save_rating($nid, $user->uid, $cid, $rate))
					$errors	=	true;
			}
		}
	}
	if($errors)
		drupal_set_message(t('There where errors while saving the ratings'),'error');
	else
		drupal_set_message(t('Ratings well saved'));
		
}

/**
 * Displays a table with the average values for every idea-criterion relation and the sumatory
 */
function ponderator_average_table_all(){
	$tid = arg(1);
	
	//Header
	$header[] 		= t('Idea');
	$averages 		= ponderator_get_average_ratings($tid);
	$criteria		= ponderator_get_criteria($tid);
	$criteriacount	=	count($criteria);
	if($criteriacount>0)
		foreach($criteria as $criterion){
			$options['attributes']['title']	=	$criterion['description'];
			$header[] = l(
							$criterion['name'].' ('.$criterion['weight'].')',
							'ponderator/criterion/'.$criterion['cid'],
							$options
						);
						/*'<a 
							href ="criterion/'.$criterion['cid'].'" 
							title="'.$criterion['description'].'">'.
							$criterion['name'].' ('.$criterion['weight'].')
						</a>';*/
		}
	else
		drupal_set_message(t('No criteria for this process'),'warning');
	$header[] = t('TOTAL');
	
	$nodes		= ponderator_get_nodes($tid);
	$nodecount	= count($nodes);
	if($nodecount>0)
		foreach($nodes as $node){
			$row = array();
			$row[] = l($node['title'],'node/'.$node['nid']);
			if($criteriacount>0)
				foreach($criteria as $criterion){
					if(isset($averages[$node['nid']][$criterion['cid']]))
						$row[] 		=	round($averages[$node['nid']][$criterion['cid']],2);
					else
						$row[] 		=	'-';
				}
			$row[] = '<b>'.round($averages[$node['nid']]['total'],2).'</b>';
			$rows[]=$row;
		}
	else{
		$rows	=	array(array());
		drupal_set_message(t('No nodes for this process'),'warning');
	}
	return theme('table',$header,$rows);
}

function ponderator_show_criterion($cid){
	$criterion		 =	ponderator_get_criterion($cid);
	if(user_access("access ponderator ".$criterion["tid"])){
		
		$header	 		= 	array();
		
		$rows[]	 		= 	array(
								'<b>'.t('Name').'</b>',
								$criterion['name']
							);
		$rows[]	 		=	array(
								'<b>'.t('Description').'</b>',
								$criterion['description']
							);
		$rows[]	 		=	array(
								'<b>'.t('Weight').'</b>',
								$criterion['weight']);
								
		$page_content	=	theme_table($header, $rows, array());
		
		$page_content	.=	l(
								t("Go to rate this criterion"),
								'ponderator/'.$criterion['tid']
							);
	}else{
		$page_content	=	'<h1>'.t("Access denied").'</h1>'.
							t('You are not authorized to access this page.');
	}
	return $page_content;
}