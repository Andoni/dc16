<?php
// $Id: ponderator_chart.page.inc, v0.2.002 2011/02/02

module_load_include('inc', 'ponderator', 'includes/ponderator.db');

function ponderator_chart_average($tid){
	$page_content	=	"";
	$node_type_str	=	variable_get('ponderator_nodetype','ideas');
	
	$criteria		=	ponderator_get_criteria($tid);
	$criteriacount	=	count($criteria);
	
	$nodes			=	ponderator_get_nodes($tid);
	$nodecount		=	count($nodes);
	
	if($nodecount<=0)
		drupal_set_message(t('No nodes for this process'),'warning');
	elseif($criteriacount<=0)
		drupal_set_message(t('No criteria for this process'),'warning');
	else{
		$averages		=	ponderator_chart_get_averages($tid,$nodes,$criteria);
		$mode			=	ponderator_get_mode($tid);
		
	    $x = round((555-7*($criteriacount+1))/(($criteriacount+1)*$nodecount))-1;//Chart height
	    if($x<5)
	    	$page_content.= '<p>'.t('The graph is to big to be displayed').'</p>';
	    else{
	    	if ($x>30)
	    		$x = 30;
	    	drupal_set_message('height='.($x*$nodecount*($criteriacount+1)+7*($criteriacount+1)+45));
	    	//Gridlines width
	    	$glw	=	100/($mode['maxvalue']-$mode['minvalue']);
	    	
		    $chart = array(
		      '#chart_id' => $tid,
		      '#title' => chart_title(t(' '), '0000ee', 15),
		      '#type' => CHART_TYPE_BAR_H_GROUPED,
		      '#size' => chart_size(500,$x*$nodecount*($criteriacount+1)+7*($criteriacount+1)+45),
		      '#grid_lines' => chart_grid_lines($glw, 10), //laukitxoak zabalera altuera
		      '#bar_size' => chart_bar_size($x, 0), //Barren Zabalera, tartea
		    );
		    $chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][0][] = chart_mixed_axis_range_label($mode['minvalue'], $mode['maxvalue']);
		    $chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('Average'), 95);
		    $chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Criteria'), 95);
			$i=0;
		    foreach($nodes as $node){
		    	$values			= $averages['ratings'][$node['nid']];	//Rating values
				$valuescount	= count($values);
				$data			= $averages['chart'][$node['nid']];		//Bar size per rating
				
				$chart['#data'][$node['title']]	= $data;
				$chart['#legends'][]			=t($node['title']);
				$chart['#data_colors'][]		= chart_unique_color($node['nid']);
				
				$j	=	0;
				if($valuescount>=0)
					foreach($values as $value){
						$chart['#shape_markers'][] = array("t $value",'000000',$i,$j,10.0);
						$j++;
					}
				else
					drupal_set_message(t("there isn't any average value"),'warning');
				$i++;
		    }
		    
		    //As the first value is the button value we must enter criterias backwards
		    $criteriareverse	=	array_reverse($criteria);
		    $chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('--TOTAL--'));
		    foreach($criteriareverse as $criterion)
		  		$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t($criterion['name']).' ('.$criterion['weight'].')');
		    $page_content .= chart_render($chart);
	    }
	}
    
	return $page_content;
}

function ponderator_chart_market($tid){
	$page_content	=	"";
	$node_type_str	=	variable_get('ponderator_nodetype','ideas');
	
	$criteriaAll	=	ponderator_get_criteria($tid);
	$criteriacount	=	count($criteriaAll);
	
	$nodes			=	ponderator_get_nodes($tid);
	$nodecount		=	count($nodes);
	
	$process		=	taxonomy_get_term($tid);
	
	if($nodecount<=0)
		drupal_set_message(t('No nodes for this process'),'warning');
	elseif($criteriacount<=0)
		drupal_set_message(t('No criteria for this process'),'warning');
	else{
	    $criteriacount = 2;
	    $criteria[0]=$criteriaAll[0];
	    $criteria[1]=$criteriaAll[1];
		
	    $averages = ponderator_chart_get_market_averages($tid, $nodes, $criteria);
	
	    $page_content.= '<h3>'.
	    					t(
	    						'Process: @processname',
	    						array('@processname'=>$process->name)
	    					).
	    				'</h3>';
	
	    $chart = array(
	      '#chart_id' => $tid,
	      '#title' => chart_title(t(' '), '0000ee', 15),
	      '#type' => CHART_TYPE_SCATTER,
	      '#size' => chart_size(525,250),
	      '#grid_lines' => chart_grid_lines(50, 50), //laukitxoak zabalera altuera
	    );
	    $chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][0][] = chart_mixed_axis_label(t('min'));
	    $chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][0][] = chart_mixed_axis_label(t('mean'));
	    $chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][0][] = chart_mixed_axis_label(t('max'));
	    $chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label($criteria[0]['name']." (".$criteria[0]['weight'].")", 95);
	    $chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t(''));
	    $chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('mean'));
	    $chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_label(t('max'));
	    $chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label($criteria[1]['name']." (".$criteria[1]['weight'].")", 95);
	
	    for($i=0;$i<$criteriacount;$i++){
		  $chart['#data'][$criteria[$i]['name']] = $averages['chart'][$criteria[$i]['cid']];
	    }
	    $color = '';
	    
	    for($i=0;$i<$nodecount;$i++){
	    	$color .= chart_unique_color($i+12);
	    	if($i!=$nodecount-1) $color .= '|';
	    	$x		=	$averages['ratings'][$criteria[0]['cid']][$nodes[$i]['nid']];
	    	$y		=	$averages['ratings'][$criteria[1]['cid']][$nodes[$i]['nid']];
			$chart['#legends'][]=$nodes[$i]['title']." ( $x / $y )";
			$chart['#shape_markers'][] = chart_shape_marker(0,$i,'o',10,chart_unique_color($i+12));
			//array('o',chart_unique_color($i+12),0,$i,10);
	    }
	    
		$chart['#data_colors'][$criteria[$i]['name']] = $color;
	    
	    $page_content .= chart_render($chart);
	}
    
	return $page_content;
}

/**
 * Get the maximun and the minimun values of ratings
 * @param $nodes
 * 	A matrix of 2 dimensions, with numerical values
 * @param $tid
 * 	process identifier
 * 
 * @return returns an object with.
 * 	-min: minimun possible value of the ratings.
 * 	-max: maximum possible value of the ratings.
 * 	-dist: distance between min & max.
 * 	-tmin: totals minimun value.
 * 	-tmax: totals maximum value.
 * 	-tdist: distance between tmin & tmax
 */
function ponderator_chart_min_max($nodes,$tid){
	$count = count($nodes);
	if($count==0){
		return null;
	}else{
		$mode		=	ponderator_get_mode($tid);
		$first_time	=	true;
		foreach($nodes as $value)
				if($first_time){
					$ret['tmin']	=	$value['total'];
					$ret['tmax']	=	$value['total'];
					$first_time	=	false;
				}elseif($value['total']<$ret['tmin'])
					$ret['tmin']	=	$value['total'];
				elseif ($value['total']>$ret['tmax'])
					$ret['tmax']	=	$value['total'];
		$ret['tdist']=$ret['tmax']-$ret['tmin'];
		
		$ret['min']	=	$mode['minvalue'];
		$ret['max']	=	$mode['maxvalue'];
		$ret['dist']=	$ret['max']-$ret['min'];
		
		return $ret;
	}
}

function ponderator_chart_get_averages($tid,$nodes,$criteria){
	$averages		=	ponderator_get_average_ratings($tid);
	
	$minMax		=	ponderator_chart_min_max($averages,$tid);
	
	foreach($nodes as $node){
		$nid	=	$node['nid'];
		foreach($criteria as $criterion){
			$cid	=	$criterion['cid'];
			if($averages[$nid][$cid]!=null || isset($averages[$nid][$cid])){
				$ret['ratings'][$nid][$cid]	=	round($averages[$nid][$cid],2);
				$value						=	(($averages[$nid][$cid]-$minMax['min'])*100/$minMax['dist']);
				if(value==0)
					$ret['chart'][$nid][$cid]	=	($value==0) ? '1' : $value;
			}else{//There is no rating average for this idea-criterion relation
				$ret['ratings'][$nid][$cid]		='-';
				$ret['chart'][$nid][$cid]	='1';
			}
		}
		if($averages[$nid]['total']=='-' || !isset($averages[$nid]['total'])){//There is no total value
			$ret['ratings'][$nid]['total']	='-';
			$ret['chart'][$nid]['total']	='1';
		}else{
			$ret['ratings'][$nid]['total']	=	round($averages[$nid]['total'],2);
			if($minMax['tdist']==0)//all totals have the same value
				$ret['chart'][$nid]['total']	=	50;
			else{
				$value							=	(($averages[$nid]['total']-$minMax['tmin'])*100/$minMax['tdist']);
				$ret['chart'][$nid]['total']	=	($value==0) ? '1' : $value;
			}
		}
	}
	return $ret;
}

function ponderator_chart_get_market_averages($tid,$nodes,$criteria){
	$ret		=	array();
	$averages	=	ponderator_get_average_ratings($tid);
	$minMax		=	ponderator_chart_min_max($averages,$tid);
	
	foreach($criteria as $criterion){
		$cid	=	$criterion['cid'];
		foreach($nodes as $node){
			$nid	=	$node['nid'];
			
			if($minMax['dist']==0||$averages[$nid][$cid]=='-'||!isset($averages[$nid][$cid]))
				$ret['chart'][$cid][$nid]	=	50;
			else
				$ret['chart'][$cid][$nid]	=	(($averages[$nid][$cid]-$minMax['min'])*100/$minMax['dist']);
				
			if($averages[$nid][$cid]=='-'||!isset($averages[$nid][$cid]))
				$ret['ratings'][$cid][$nid]	=	'-';
			else
				$ret['ratings'][$cid][$nid]	=	round($averages[$nid][$cid],2);
				
				
		}
	}
	return $ret;
}