<?php
// $Id: ponderator.db.inc, v0.2.002 2011/02/02

/******	All functions that interact with the database	******/

/**************************	Getters	**************************/

/**
 * Returns a matrix with the average values (the dimensions are the node identifier and criterion identifier)
 * @param $tid
 * 	Process identifier
 */
function ponderator_get_average_ratings($tid){
	$query = "
		SELECT 
			pr.nid,
			pr.cid,
			pc.weight,
			AVG(pr.rate) as average
		FROM
			ponderator_rate as pr,
			ponderator_criterion as pc
		WHERE
			pr.cid=pc.cid AND
			pc.tid = $tid
		GROUP BY
			pr.cid,
			pr.nid
		ORDER BY
			pr.nid,
			pr.cid";
	$query_result = db_query($query);
	while($result = db_fetch_object($query_result)){
		$nid	=	$result->nid;
		$cid	=	$result->cid;
		$avg	=	$result->average;
		$weight	=	$result->weight;
		if(!isset($totals[$nid]))
			$totals[$nid]	= '-';
		if(isset($avg)){
			$ret[$nid][$cid]	=	$avg;
			if($totals[$nid]=='-')
				$totals[$nid] =		$avg * $weight;
			else
				$totals[$nid] +=	$avg * $weight;
		}
	}
	
	if(isset($ret)){
		//add totals in the end of each $ret[$nid] array
		$nids=array_keys($ret);
		foreach($nids as $nid){
			$ret[$nid]['total']=$totals[$nid];
		}
	}
	return $ret;
}

function ponderator_get_criteria($tid){
	$ret=null;
	$sql = "
		SELECT
			cid,
			name,
			description,
			weight
		FROM
			ponderator_criterion
		WHERE
			tid=".$tid;
	$query_result = db_query($sql);
	$i=0;
	while($result = db_fetch_object($query_result)){
		$ret[$i]['cid']=$result->cid;
		$ret[$i]['name']=$result->name;
		$ret[$i]['description']=$result->description;
		$ret[$i]['weight']=$result->weight;
		$i++;
	}
	return $ret;
}

/**
 * Returns the number of criteria of each process.
 */
function ponderator_get_criteria_number(){
	$query = "
		SELECT
			tid,
			COUNT(cid) as count
		FROM
			ponderator_criterion
		GROUP BY
			tid";
	$query_result = db_query($query);
	while($result = db_fetch_object($query_result))
		$cn[$result->tid] = $result->count;
	return $cn;
}

/**
 * Returns a concrete criterion
 * @param $cid
 * 	Criterion identifier
 */
function ponderator_get_criterion($cid){
	$sql = "
		SELECT
			cid,
			tid,
			name,
			description,
			weight
		FROM
			ponderator_criterion
		WHERE
			cid=".$cid;
	$query_result = db_query($sql);
	if($result = db_fetch_object($query_result)){
		$ret['cid']=$result->cid;
		$ret['tid']=$result->tid;
		$ret['name']=$result->name;
		$ret['description']=$result->description;
		$ret['weight']=$result->weight;
	}else{
		$ret['cid']=$cid;
		$ret['tid']='-';
		$ret['name']=t('Unknown');
		$ret['description']=t('Criteria does not exist');
		$ret['weight']='-';
	}
	return $ret;
}

/**
 * Take the saved ponderation mode
 * @param $tid
 * 	Process identifier
 */
function ponderator_get_mode($tid){
	$query = "
		SELECT
			type,
			name,
			description,
			value
		FROM
			ponderator_mode
		WHERE
			tid=".$tid;
	$query_result = db_query($query);
	if($result = db_fetch_object($query_result)){
		$posible_modes			= ponderator_get_posible_modes($result->value,$tid);
		$mode['type']			= $result->type;
		$mode['name']			= $result->name;
		$mode['description']	= $result->description;
		$mode['value']			= $result->value;
		$mode['minvalue']		= $posible_modes[$result->type]['minvalue'];
		$mode['maxvalue']		= $posible_modes[$result->type]['maxvalue'];
	}else{
		$posible_modes			= ponderator_get_posible_modes();
		$mode['type']			= 'N';
		$mode['name']			= $posible_modes['N']['name'];
		$mode['description']	= $posible_modes['N']['description'];
		$mode['value']			= $posible_modes['N']['values'];
		$mode['minvalue']		= $posible_modes['N']['minvalue'];
		$mode['maxvalue']		= $posible_modes['N']['maxvalue'];
	}
	return $mode;
}

/**
 * Takes all the ratins of a user in a process.
 * 
 * @param $uid
 * 	user identifier
 * @param $tid
 * 	Process identifier
 */
function ponderator_get_ratings($uid,$tid){
	$query = "
		SELECT 
		pr.nid,
		pr.cid,
		pr.rate
	FROM
		ponderator_rate as pr,
		ponderator_criterion as pc
	WHERE
		pr.cid=pc.cid AND
		pc.tid = ".$tid." AND
		pr.uid = ".$uid;
	$query_result = db_query($query);
	while($result = db_fetch_object($query_result)){
		$ret[$result->nid][$result->cid]	=	$result->rate;
	}
	return $ret;
}

/**
 * Returns array of ideas (nid and title)
 * 
 * @param $node_type_str
 * 	A string with the node type
 * @param $tid
 * 	Process identifier, -1 if there is no processes
 */
function ponderator_get_nodes($tid){
	$node_type_str = variable_get('ponderator_nodetype','ideas');
	if(!$tid)
		return array();
	elseif($tid!=-1)
  		$query = "
  			SELECT DISTINCT 
  				n.nid,
  				n.title 
  			FROM
  				node AS n, 
  				term_node AS t 
  			WHERE
  				t.nid = n.nid AND 
  				t.tid = $tid AND
  				n.type = '$node_type_str'";
	else
  		$query = "
  			SELECT DISTINCT 
  				n.nid,
  				n.title 
  			FROM 
  				node AS n 
  			WHERE 
  				n.type = '$node_type_str'";
  	$query_result_nodes = db_query($query);
	while($node = db_fetch_object($query_result_nodes))
    	$ret[]	=	array(
    		'nid'	=>	$node->nid,
    		'title' =>	$node->title,
    	);
	return $ret;
}

/**************************	Setters	**************************/
/**
 * Saves a concrete rating
 * 
 * @param $nid
 * 	node (idea) identifier.
 * @param $uid
 * 	User identifier.
 * @param $cid
 * 	Criteria identifier.
 * @param $rate
 * 	The rating value.
 */
function ponderator_save_rating($nid,$uid,$cid,$rate){
	$rate = ($rate=="") ? 'null' : $rate;
		$sql = "
			REPLACE INTO ponderator_rate(
				nid,
				uid,
				cid,
				rate
			) 
			VALUES(".
				$nid.", ".
				$uid.", ".
				$cid.", ".
				$rate.")";
    return db_query($sql);
}

/**
 * Save or update a criterion in the database
 * 
 * @param $cid
 * 	Criterion identifier (null if the criterion is new)
 * @param $tid
 * 	Process identifier
 * @param $name
 * 	Criterion name
 * @param $description
 * 	Criterion description
 * @param $weight
 * 	Criterion weight
 */
function ponderator_save_criterion($cid=null,$tid,$name,$description,$weight){
	$cid = ($cid==null) ? 'null' : $cid;
		$sql = "
			REPLACE INTO ponderator_criterion(
				cid,
				tid, 
				name, 
				description, 
				weight
			) 
			VALUES(".
				$cid.", ".
				$tid.", '".
				$name."', '".
				$description."', ".
				$weight.")";
    return db_query($sql);
}

/**
 * Save the ponderation mode in the database
 * @param $tid 
 * 	process identifier
 * @param $type 
 * 	ponderation type ('N', 'MC' or 'SP')
 * @param $name
 * 	ponderation name
 * @param $description
 * 	ponderation description
 * @param $values
 *  ponderation values
 */
function ponderator_save_mode($tid,$type,$name,$description,$values){
		$sql = "
			REPLACE INTO ponderator_mode(
				tid,
				type, 
				name, 
				description, 
				value
			) 
			VALUES(".
				$tid.", '".
				$type."', '".
				$name."', '".
				$description."', '". 
				$values."')";
    return db_query($sql);
}

/**************************	Deleters	**************************/

/**
 * Function that deletes a criterion from de data base
 * 
 * @param $cid
 * 	Criterion identifier
 */
function ponderator_delete_criterion($cid){
	$query = "
		DELETE FROM
			ponderator_criterion
		WHERE
			cid=".$cid;
	db_query($query);
}