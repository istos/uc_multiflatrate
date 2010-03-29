<?php

/**
 * @file
 * Administration menu items for multiple flat rates based on product classes.
 */

/**
 * List and compare all flatrate shipping quote methods.
 */
function uc_multiflatrate_admin_methods() {
  $output = '';

  $context = array(
    'revision' => 'themed',
    'type' => 'amount',
  );

  $rows = array();
  $enabled = variable_get('uc_quote_enabled', array());
  $weight = variable_get('uc_quote_method_weight', array());
  $result = db_query("SELECT mid, title, label FROM {uc_multiflatrate_methods}");
  while ($method = db_fetch_object($result)) {
    $row = array();
    $row[] = check_plain($method->title);
    $row[] = check_plain($method->label);

    $row[] = l(t('edit'), 'admin/store/settings/quotes/methods/multiflatrate/'. $method->mid);
    $row[] = l(t('conditions'), CA_UI_PATH .'/uc_multiflatrate_get_quote_'. $method->mid .'/edit/conditions');
    $rows[] = $row;
  }
  if (count($rows)) {
    $header = array(t('Title'),t('Label'),  array('data' => t('Operations'), 'colspan' => 2));
    $output .= theme('table', $header, $rows);
  }
  $output .= l(t('Add a new multiflat rate shipping method.'), 'admin/store/settings/quotes/methods/multiflatrate/add');
  return $output;
}


function uc_multiflatrate_admin_method_manager($mid = 0){
 

  $output .= drupal_get_form('uc_multiflatrate_admin_method_edit_form', $mid);

  if ($mid == 'add'){}else{

    $output .= t('Existing product class rules');


    $context = array(
      'revision' => 'themed',
      'type' => 'amount',
    );
  
    //Create table of existing product class rates for this method
    $result = db_query("SELECT mid, pcid, base_price, base_unit, additional_unit_price FROM {uc_multiflatrate_product_class} WHERE mid = %d", $mid);
    while ($product_method = db_fetch_object($result)){
      $row = array();
      $row[] = check_plain($product_method->pcid);
      $row[] = uc_price($product_method->base_price, $context);
      $row[] = $product_method->base_unit;
      $row[] = uc_price($product_method->additional_unit_price, $context);
      $row[] = l(t('delete'), 'admin/store/multiflatrate/product_class/'. $product_method->mid.'/delete/'.$product_method->pcid);
      $row[] = l(t('edit'), 'admin/store/multiflatrate/product_class/'. $product_method->mid.'/edit/'.$product_method->pcid);
      $rows[] = $row;
    }
    if (count($rows)) {
      $header = array(t('Product Class'),t('Base Price'), t('Base Unit'), t('Additional Unit Price'), array('data' => t('Operations'), 'colspan' => 2));
      $output .= theme('table', $header, $rows);
    }
 
    $output .= drupal_get_form('uc_multiflatrate_admin_product_class_edit_form', $mid);
  }
   
  return $output;
  
}




function uc_multiflatrate_admin_product_class_edit_form($form_state, $mid = 0, $pcid = '', $edit = false){
	
  $form = array();
  
  if (is_numeric($mid) && ($method = db_fetch_object(db_query("SELECT * FROM {uc_multiflatrate_methods} WHERE mid = %d", $mid)))) {
    $form['mid'] = array('#type' => 'value', '#value' => $mid);
  }
  
  $current_product_class = db_fetch_object(db_query("SELECT * FROM {uc_multiflatrate_product_class} WHERE mid = %d AND pcid LIKE '%s'", $mid, $pcid));
    $form['pcid'] = array('#type' => 'value', '#value' => $pcid);
  
  $sign_flag = variable_get('uc_sign_after_amount', FALSE);
  $currency_sign = variable_get('uc_currency_sign', '$');
  
  //Get all the product classes available for which rates have not been set yet - only show those to the user 
  $query = db_query("SELECT pcid FROM {uc_multiflatrate_product_class} WHERE mid = %d", $mid);
  $product_classes = uc_product_types();
  $product_classes = array_combine($product_classes, $product_classes);
  $existing_product_classes = array();
  while ($defined_product_class = db_fetch_object($query)){
    $existing_product_classes[$defined_product_class->pcid] = $defined_product_class->pcid;
  }
  
  //Unless we are editing an existing rule remove the product classes already defined
  if (!$edit){ 
    foreach ($existing_product_classes as $product_class){
      if (in_array($product_class, $product_classes)){
        unset($product_classes[$product_class]);
      }
    }
  }
  if (!$edit){  
    $form['product_class'] = array(
      '#title' => t('Add product class rule'),
      '#type' => 'fieldset',
    );
    $form['product_class']['product_class_id'] = array(
    	'#type' => 'select',
      '#title' => t('Product Class'),
      '#description' => t('The product class this rate should apply to'),
      '#default_value' => $pcid,
      '#options' =>  $product_classes,
      '#required' => TRUE,
    );
  }
  else{
    $form['product_class'] = array(
      '#title' => t('Edit product class rule: '). $pcid,
      '#type' => 'fieldset',
    );    
  }
  
  $form['product_class']['base_price'] = array(
  	'#type' => 'textfield',
    '#title' => t('Base price'),
    '#description' => t('The starting price for shipping costs.'),
    '#default_value' => $current_product_class->base_price,
    '#size' => 16,
    '#field_prefix' => $sign_flag ? '' : $currency_sign,
    '#field_suffix' => $sign_flag ? $currency_sign : '',
  );
  $form['product_class']['base_unit'] = array(
  	'#type' => 'textfield',
    '#title' => t('Base unit (number of products)'),
    '#default_value' => $current_product_class->base_unit,
    '#size' => 16,
  );
  $form['product_class']['additional_unit_price'] = array(
  	'#type' => 'textfield',
    '#title' => t('For every additional unit add'),
    '#default_value' => $current_product_class->additional_unit_price,
    '#size' => 16,
    '#field_prefix' => $sign_flag ? '' : $currency_sign,
    '#field_suffix' => $sign_flag ? $currency_sign : '',
  );

  if (!$edit){
    $form['product_class']['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add product class rule'),
    );
  }else{
    $form['product_class']['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save product class rule'),
    );    
  }
  
  return $form;
  
}


function uc_multiflatrate_admin_product_class_edit_form_validate($form, &$form_state){

  if ($form_state['values']['mid']==0){
    form_set_error('product_class', t('Please save the method before adding product class rules'));
  }

  if ($form_state['values']['op'] == t('Add product class rule')) {
    if (!empty($form_state['values']['base_price']) && !is_numeric($form_state['values']['base_price'])) {
      form_set_error('base_price', t('The base rate must be a numeric amount.'));
    }
    if (!empty($form_state['values']['additional_unit_price']) && !is_numeric($form_state['values']['additional_unit_price'])) {
      form_set_error('additional_unit_price', t('The additional unit price must be a numeric amount.'));
    }
    if (!empty($form_state['values']['base_unit']) && !is_numeric($form_state['values']['base_unit'])) {
      form_set_error('base_unit', t('The base unit must be a numeric amount.'));
    }
  }
  
}


function uc_multiflatrate_admin_product_class_edit_form_submit($form, &$form_state){

  if ($form_state['values']['pcid']!='') {
    db_query("UPDATE {uc_multiflatrate_product_class} SET base_price = %f,
             base_unit = %d, additional_unit_price = %f WHERE mid = %d AND pcid like '%s'",
      $form_state['values']['base_price'], $form_state['values']['base_unit'],
      $form_state['values']['additional_unit_price'], $form_state['values']['mid'], $form_state['values']['pcid']);
    drupal_set_message(t("Multiflat rate product rates updated."));
    drupal_goto('admin/store/settings/quotes/methods/multiflatrate/' .$form_state['values']['mid']);
  }
  else{
    db_query("INSERT INTO {uc_multiflatrate_product_class} (mid, pcid, base_price, base_unit, additional_unit_price)
             VALUES ('%s', '%s', %f, %d, %f)",
      $form_state['values']['mid'], $form_state['values']['product_class_id'], $form_state['values']['base_price'],
      $form_state['values']['base_unit'], $form_state['values']['additional_unit_price']);  
    drupal_set_message(t("Added new multiflat product rate."));    
  }
  
}


function uc_multiflatrate_admin_method_edit_form($form_state, $mid = 0) {
  
  if (is_numeric($mid) && ($method = db_fetch_object(db_query("SELECT * FROM {uc_multiflatrate_methods} WHERE mid = %d", $mid)))) {
    $form['mid'] = array('#type' => 'value', '#value' => $mid);
  }
  
  $form['title'] = array('#type' => 'textfield',
    '#title' => t('Shipping method title'),
    '#description' => t('The name shown to distinguish it from other multilatrate methods.'),
    '#default_value' => $method->title,
    '#required' => TRUE,
  );
  $form['label'] = array('#type' => 'textfield',
    '#title' => t('Line item label'),
    '#description' => t('The name shown to the customer when they choose a shipping method at checkout.'),
    '#default_value' => $method->label,
    '#required' => TRUE,
  );
  
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );  
  $form['buttons']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('uc_multiflatrate_admin_method_edit_form_delete'),
  );

  return $form;
}



/**
 * 
 */
function uc_multiflatrate_admin_method_edit_form_validate($form, &$form_state) {
}

/**
 * @see uc_multiflatrate_admin_method_edit_form()
 */
function uc_multiflatrate_admin_method_edit_form_delete($form, &$form_state) {
  drupal_goto('admin/store/settings/quotes/multiflatrate/'. $form_state['values']['mid'] .'/delete');
}

/**
 * @see uc_multiflatrate_admin_method_edit_form()
 */
function uc_multiflatrate_admin_method_edit_form_submit($form, &$form_state) {
  if ($form_state['values']['mid']) {
    db_query("UPDATE {uc_multiflatrate_methods} SET title = '%s', label = '%s' WHERE mid = %d",
      $form_state['values']['title'], $form_state['values']['label'], $form_state['values']['mid']);
    drupal_set_message(t("Multiflat rate shipping method was updated."));
  }
  else {
    db_query("INSERT INTO {uc_multiflatrate_methods} (title, label) VALUES ('%s', '%s')",
      $form_state['values']['title'], $form_state['values']['label']);

    $mid = db_last_insert_id('uc_multiflatrate_methods', 'mid');
    $enabled = variable_get('uc_quote_enabled', array());
    $enabled['multiflatrate_'. $mid] = TRUE;
    variable_set('uc_quote_enabled', $enabled);
    $weight = variable_get('uc_quote_method_weight', array());
    $weight['multiflatrate_'. $mid] = 0;
    variable_set('uc_quote_method_weight', $weight);
    drupal_set_message(t("Created and enabled new multiflat rate shipping method."));
  }
  $form_state['redirect'] = 'admin/store/settings/quotes/methods/multiflatrate';
}

/******************************************************************************
 * Menu Callbacks                                                             *
 ******************************************************************************/

function uc_multiflatrate_admin_method_product_class_edit($mid, $pcid){
  
  $output = drupal_get_form('uc_multiflatrate_admin_product_class_edit_form', $mid, $pcid, true) ;
  return $output;
}



function uc_multiflatrate_admin_method_product_class_delete($mid, $pcid){  
  $output = drupal_get_form('uc_multiflatrate_admin_method_product_class_confirm_delete', $mid, $pcid) ;
  return $output;
}

function uc_multiflatrate_admin_method_product_class_confirm_delete($form_state, $mid, $pcid){
  
  $form = array();
  $form['mid'] = array('#type' => 'value', '#value' => $mid);
  $form['pcid'] = array('#type' => 'value', '#value' => $pcid);

  return confirm_form($form, t('Do you want to delete the rate related to this product class?'),
    'admin/store/settings/quotes/methods/multiflatrate/'.$mid,
    t('Delete rates for product of type: '.$pcid));
    
}

function uc_multiflatrate_admin_method_product_class_confirm_delete_submit($form, &$form_state){
    db_query("DELETE FROM {uc_multiflatrate_product_class} WHERE mid = %d AND pcid LIKE '%s'", $form_state['values']['mid'], $form_state['values']['pcid']);
    drupal_set_message(t('Multiflat rate rule deleted.'));
    $form_state['redirect'] = 'admin/store/settings/quotes/methods/multiflatrate/' . $form_state['values']['mid'];
}


/**
 * Confirm deletion of a multiflat rate shipping method.
 *
 * @see uc_multiflatrate_admin_method_confirm_delete_submit()
 */
function uc_multiflatrate_admin_method_confirm_delete($form_state, $mid) {
  $form = array();
  $form['mid'] = array('#type' => 'value', '#value' => $mid);

  return confirm_form($form, t('Do you want to delete this shipping method? This will
                               delete all associated product class prices as well1.'),
    'admin/store/settings/quotes/methods/multiflatrate',
    t('Delete'));
}

/**
 * @see uc_multiflatrate_admin_method_confirm_delete()
 */
function uc_multiflatrate_admin_method_confirm_delete_submit($form, &$form_state) {
  db_query("DELETE FROM {uc_multiflatrate_methods} WHERE mid = %d", $form_state['values']['mid']);
  
  db_query("DELETE FROM {uc_multiflatrate_product_class} WHERE mid = %d", $form_state['values']['mid']);
  
  ca_delete_predicate('uc_multiflatrate_get_quote_'. $form_state['values']['mid']);

  $enabled = variable_get('uc_quote_enabled', array());
  unset($enabled['flatrate_'. $form_state['values']['mid']]);
  variable_set('uc_quote_enabled', $enabled);

  $weight = variable_get('uc_quote_method_weight', array());
  unset($weight['flatrate_'. $form_state['values']['mid']]);
  variable_set('uc_quote_method_weight', $weight);

  drupal_set_message(t('Multiflat rate shipping method deleted.'));
  $form_state['redirect'] = 'admin/store/settings/quotes/methods/multiflatrate';
}

