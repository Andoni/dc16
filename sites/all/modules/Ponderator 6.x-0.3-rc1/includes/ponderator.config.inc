<?php
// $Id: ponderator.cofnig.inc, v0.3.001 2011/02/07

module_load_include('inc', 'ponderator', 'includes/ponderator.db');

/**
 * The maximum criterion weight allowed
 */
define("MAXWEIGHT",5);

/**
 * The minimum criterion weight allowed
 */
define("MINWEIGHT",-5);

/**
 * The muximum number of criteria allowed
 */
define("MAXCRITERIA",5);

/**
 * Maximum length of a criterion name
 */
define("MAXCNAME",32);

function ponderator_config_list(){
	$ret="";
	$header = array(
		t("id"),
		t("Process"),
		t("Number of criteria"),
		t("Edit"),
	);
	
	$processes = taxonomy_get_tree(variable_get("ponderator_vid","0"));
	$criteria_number	=	ponderator_get_criteria_number();
	foreach($processes as $process){
		if(user_access('access ponderator '.$process->tid))
			$rows[] = array(
				$process->tid,
				$process->name,
				isset($criteria_number[$process->tid]) ? $criteria_number[$process->tid] : t('no criteria set'),
				l(t('edit'),'ponderator-config/'.$process->tid),
			);
	}
	
	$ret =  theme('table',$header,$rows);
	return $ret;
}

/**
 * Process configuration form
 * 
 * @param $form_state
 * @param $tid
 * 	Process identifier
 */
function ponderator_config_form(&$form_state,$tid){
	$t 				= taxonomy_get_term($tid);
	$first_time		= !isset($form_state['values']['tid']);
	$criteria_count = 1;
	$form['config'] = array(
    	'#type'				=> 'fieldset',
    	'#title'			=> $t->name,
	);
	$form['config']['tid'] = array(
		'#type'				=> 'hidden',
		'#value'			=> $tid,
	);
	
	//Ponderation mode
	if(	$first_time || 
		$form_state['values']['mode-values']=='1-10' || 
		$form_state['values']['mode-values']=='1-# of ideas' ||
		$form_state['values']['mode-values']==''){
		$mode = ponderator_get_mode($tid);
		if($mode['type']!='MC')
			$mode['value']	= '1,3,9';
	}else{
		$mode['value'] = $form_state['values']['mode-values'];
	}
	if(!$first_time)
		$mode['type']	=	$form_state['values']['mode'];
		
	$form['config']['mode'] = array(
		'#type' 			=> 'select',
		'#title' 			=> t('Ponderation mode'),
		'#default_value' 	=> $mode['type'],
		'#options' 			=> ponderator_get_posible_modes_names(),
		'#description' 		=> t('Ponderation mode.'),
	    '#multiple' 		=> false,
	    '#ahah' => array(
	      'event' 	=> 'change',
	      'path' 	=> 'ponderator-config/'.$tid.'/js/change-mode',
	      'wrapper'	=> 'mode-values-wrapper',
	      'method' 	=> 'replace',
	    ),
	);
	$form['config']['mode-values'] = ponderator_config_form_mode($mode);

	//Criteria
	if ($first_time){
		$criteria = ponderator_get_criteria($tid);
		$criteria_count = count($criteria)+1;
	}elseif(arg(3)=='new-criterion')
		$criteria_count = count($form_state['values']['criteria'])+1;
	else
		$criteria_count = count($form_state['values']['criteria']);
	
	if($criteria_count>MAXCRITERIA){
		$criteria_count=MAXCRITERIA;
		drupal_set_message(
			t(
				"There can not be more than @maxcriteria criteria",
				array("@maxcriteria"=>MAXCRITERIA)
			)
		);
	}
	
	$form['config']['criteria'] = array(
		'#type'				=> 'fieldset',
		'#title'			=> t('Criteria'),
		'#description'		=> t('Leave the criteria name empty to erase it'),
		'#prefix'			=> '<div id="config-criteria">',
		'#tree'				=> TRUE,
		'#collapsible'		=> TRUE,
		'#collapsed'		=> FALSE
	);
	
	for($delta=0;$delta < $criteria_count; $delta++) {
		if(isset($form_state['values']['criteria'][$delta]['name'])){
			$cid 			= $form_state['values']['criteria'][$delta]['cid'];
			$name 			= $form_state['values']['criteria'][$delta]['name'];
			$description 	= $form_state['values']['criteria'][$delta]['description'];
			$weight 		= $form_state['values']['criteria'][$delta]['weight'];
		}elseif (isset($criteria[$delta]['name'])){
			$cid 			= $criteria[$delta]['cid'];
			$name 			= $criteria[$delta]['name'];
			$description 	= $criteria[$delta]['description'];
			$weight 		= $criteria[$delta]['weight'];
		}else{
			$cid 			= null;
			$name 			= '';
			$description 	= '';
			$weight 		= '1';
		}
		$form['config']['criteria'][$delta] = ponderator_config_form_criteria($delta, $cid, $name, $description, $weight);
	}
	
	if($criteria_count>=MAXCRITERIA){
		$disabled=	true;
		$value	=	t(
				"There can not be more than @maxcriteria criteria",
				array("@maxcriteria"=>MAXCRITERIA)
			);
	}else{
		$disabled	=	false;
		$value		=	t('Add Criterion');
	}
    $form['config']['ahah'] = array(
	    '#type' 	=> 'submit',
    	'#title'	=> t('Add'),
	    '#value' 	=> $value,
    	'#disabled'	=> $disabled,
		'#suffix'	=> '</div>',
	    //'#weight'	=> 0,
    	//'#submit'	=> 'ponderator_more_criteria_submit', //no JS
	    '#ahah' => array(
		    'event' 	=> 'click',
		    'path' 		=> 'ponderator-config/'.$tid.'/js/new-criterion',
		    'wrapper' 	=> 'config-criteria',
		    'method' 	=> 'replace',
		    'effect' 	=> 'fade',
	    	'progress' 	=> array(
		        'type' 		=> 'bar',
		        'message' 	=> t('Loading...')
	    	)     
	    ),
	);
    $form['config']['submit'] = array(
    	'#prefix'	=> "<br/>",
		'#title'	=> t('Save'),
		'#type'		=> 'submit',
		'#value'	=> t('Save'),
    	'#weight'	=> 1,
    );
    
    return $form;
}

function ponderator_config_form_mode($mode){
	$ppm		= ponderator_get_posible_modes($mode['value']);
	
	$prefix 		= '<div id="mode-values-wrapper">';
	$suffix 		= '</div>';
	$description	= $ppm[$mode['type']]['description'];
	$default_value	= $ppm[$mode['type']]['values'];
	$title			= $ppm[$mode['type']]['name'];
	
	if($mode['type']=='MC')
		$disabled	= false;
	else
		$disabled	= true;
		
	return array(
		'#type'			=>	'textfield',
		'#title'		=>	$title.t(' posible values'),
		'#description'	=>	$description,
		'#default_value'=>	$default_value,
		'#disabled'		=>	$disabled,
		'#prefix'		=>	$prefix,
		'#suffix'		=>	$suffix
	);
}
/**
 * Criterion adding function
 * 
 * @param $delta
 * 	Criterion list number
 * @param $name
 * 	Criterion name
 * @param $description
 * 	Criterion description
 * @param $weight
 * 	Criterion weight
 */
function ponderator_config_form_criteria($delta, $cid=null, $name="", $description="", $weight=1) {
	$form = array(
		'#tree' => TRUE,
	);
	$collapsed = ($name==""||$weight=="") ? false : true;
	$form['new-criterion'] = array(
		'#type'				=> 'fieldset',
		'#title'			=> t(	'Criterion @n: @name (@weight)',
									array(
										'@n' 		=>	($delta + 1),
										'@name'		=>	$name,
										'@weight'	=>	$weight,
									)
								),
		'#prefix'			=> '<div id="config-criteria">',
		'#suffix'			=> '</div>',
		'#collapsible'		=> true,
		'#collapsed'		=> $collapsed,
	);
	
	$form['new-criterion']['cid'] = array(
		'#type' 			=> 'hidden',
		'#value'			=> $cid,
		'#parents' 			=> array('criteria', $delta,'cid'),
	);
	$form['new-criterion']['name'] = array(
		'#type' 			=> 'textfield',
		'#title' 			=> t('Name'),
		'#description'		=> t('Leave this field empty to erase the criterion'),
		'#default_value' 	=> $name,
		'#parents' 			=> array('criteria', $delta,'name'),
	);
	
	$form['new-criterion']['description'] = array(
		'#type' 			=> 'textarea',
		'#title' 			=> t('Description'),
		'#default_value' 	=> $description,
		'#rows'				=> 1,
		'#parents' 			=> array('criteria',$delta,'description'),
	);
	
	$form['new-criterion']['weight'] =array(
		'#type' 			=> 'textfield',
		'#title' 			=> t('Weight'),
		'#description'		=> t('Must be between <b>'.MINWEIGHT.'</b> and <b>'.MAXWEIGHT.'</b>'),
		'#default_value' 	=> $weight,
		'#size' 			=> 5,
		'#maxlength' 		=> 2,
		'#parents' 			=> array('criteria',$delta,'weight'),
	);

	return $form;
}

/**
 * Config form validation
 * 
 * @param $form
 * @param $form_state
 */

function ponderator_config_form_validate($form,&$form_state){
	if($form_state['clicked_button']['#value']==t('Save')){
		$mode			=	$form_state['values']['mode'];
		$mode_values	=	$form_state['values']['mode-values'];
		$values			=	explode(',', $mode_values);
		$field_name		=	'mode-values';
		if($mode=='MC'){
			if($mode_values=='')
	        	form_set_error(
	          		$field_name, 
	          		t('Values field can not be empty')
	          	);
	        else
				foreach($values as $value)
					if(!is_numeric($value))
			        	form_set_error(
			          		$field_name, 
			          		t('You must enter only numeric values separated by ","')
			          	);
		}
		$criteria	=	$form_state['values']['criteria'];
		$count		=	count($criteria);
		for($delta=0;$delta<$count;$delta++){
			$field_name	=	"criteria][$delta][weight";
			$value		=	$criteria[$delta]['weight'];
			$cname	=	str_replace(' ', '',$criteria[$delta]['name']);
			if($cname!='')
				if(!is_numeric($value))
		        	form_set_error(
		          		$field_name, 
		          		t('Weight must be numeric')
		          	);
		        elseif($value<MINWEIGHT || $value>MAXWEIGHT)
		        	form_set_error(
		          		$field_name, 
		          		t(
		          			'Weight must be between @minweight and @maxweight',
		          			array(
		          				'@minweight'=>MINWEIGHT,
		          				'@maxweight'=>MAXWEIGHT
		          			)
		          		)
		          	);
		        elseif($value==0)
		        	form_set_error(
		          		$field_name, 
		          		t('Weight can not be 0')
		          	);
		    elseif ($cname['lenght']>MAXCNAME){
		        	form_set_error(
		          		$field_name, 
		          		t(
		          			'Criteria name can not be longer than @maxcname',
		          			array(
		          				'@maxcname'=>MAXCNAME
		          			)
		          		)
		          	);
		    }
		}
	}
}

/**
 * Config form save function
 * 
 * @param $form
 * @param $form_state
 */
function ponderator_config_form_submit($form, &$form_state) {
	if($form_state['clicked_button']['#value']==t('Save')){
		$posible_modes 			= ponderator_get_posible_modes($form_state['values']['mode-values']);
		$mode_ok = ponderator_save_mode(
			$form_state['values']['tid'],
			$form_state['values']['mode'],
			$posible_modes[$form_state['values']['mode']]['name'],
			$posible_modes[$form_state['values']['mode']]['description'],
			$posible_modes[$form_state['values']['mode']]['values']
		);
		$criteria_ok 		= true;
		$failed_criteria	= '|';
		foreach($form_state['values']['criteria'] as $criterion){
			$name	=	str_replace(' ', '',$criterion['name']);
			if($name!=""){
				if(!ponderator_save_criterion(
					$criterion['cid'], 
					$form_state['values']['tid'], 
					$criterion['name'], 
					$criterion['description'], 
					$criterion['weight']
				)){
					$criteria_ok 		= false;
					$failed_criteria	= $criterion['name'].'|';
				}else
					drupal_set_message(t('Criterion saved: ').$criterion['name']);
			}elseif($criterion['cid']!=null){
				if(ponderator_delete_criterion($criterion['cid']))
					drupal_set_message(t("deleted criterion: ").$criterion['cid']);
			}
		}
		if($mode_ok && $criteria_ok)
			drupal_set_message(t('Configuration successfully saved'));
		else{
			if(!$mode_ok)
				drupal_set_message(t("Ponderation mode couldn't be saved"),'error');
			if(!$criteria_ok)
				drupal_set_message(t("Criteria saving failed on criteria: ").$failed_criteria,'error');
		}
	}
}

/**
 * AHAH function
 * 
 */
function ponderator_config_js($tid,$action){
	
	$form_state = array('storage' => NULL, 'submitted' => FALSE);
	$form_build_id = $_POST['form_build_id'];
	$form = form_get_cache($form_build_id, $form_state);
	
	$args = $form['#parameters'];
	$form_id = array_shift($args);
	$form_state['post'] = $form['#post'] = $_POST;
	$form['#programmed'] = $form['#redirect'] = FALSE;
	
	drupal_process_form($form_id, $form, $form_state);
	$form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
	
	switch ($action){
		case 'new-criterion':
			$choice_form[] = $form['config']['criteria'];
			$choice_form[] = $form['config']['ahah'];
		
			break;
		case 'change-mode':
			$choice_form[] = $form['config']['mode-values'];
			break;
		default:
			drupal_set_message('ponderator_config_js: No valid arguments','error');
	}
	
	//unset($choice_form['#prefix'], $choice_form['#suffix']);
	//$output =	theme('status_messages').drupal_render($choice_form);
	foreach($choice_form as $choice){
			unset($choice['#prefix'], $choice['#suffix']);
			$output .=	drupal_render($choice);
	}
	
	drupal_json(array('status' => TRUE, 'data' => $output));
}


/****************************************************************/

/**
 * Get only the names of posible modes
 * @return
 * 	array of posible mode names $array[mode_key] = mode_name
 */
function ponderator_get_posible_modes_names(){
	$ret	=	array();
	$pms	=	ponderator_get_posible_modes();
	$keys	=	array_keys($pms);
	foreach($keys as $key)
		$ret[$key]	=	$pms[$key]['name'];
	return $ret;
}