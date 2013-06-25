/**
 * @file ubercontent.inc
 * 		Helper functions for the ubercontent module
 */

/*
 * _ubercontent_node_add_field
 * Created by: mandelbro
 * adds a Field field to a noded
 *
 * @param $type
 * 		Content type object
 * @param $field_name
 * 		The field machine name
 * @param $label
 * 		Field label
 * @param $options
 *		Field options to send to field_create_instance
 */
function _ubercontent_node_add_field($type, $field_name, $options = NULL) {
  $field = field_info_field($field_name);
  $instance = field_info_instance('node', $field_name, $type->type);
  if (empty($field)) {
  	$field = array(
  		'cardinality' => isset($options['cardinality']) ? $options['cardinality'] : 1,
			'field_name' => $field_name,
			'entity_types' => array('node'),
			'type' => !empty($options['type']) ? $options['type'] : 'text_with_summary',
			'settings' => isset($options['field_settings']) ? $options['field_settings'] : array(),
    );

    $field = field_create_field($field);
  }
  if (empty($instance)) {
    $instance = array(
      'field_name' => $field_name,
      'entity_type' => 'node',
      'bundle' => $type->type,
      'label' => $options['label'],
      'required' => isset($options['required']) ? $options['required'] : FALSE,
      'widget' => isset($options['widget']) ? $options['widget'] : array('type' => 'text_with_summary'),
      'settings' => isset($options['settings']) ? $options['settings'] : array('display_summary' => TRUE),
      'default_value_function' => isset($options['default_value_function']) ? $options['default_value_function'] : '',
      'default_value' => !isset($options['default_value']) ? NULL : array(
      	array('value' => $options['default_value']),
      ),
      'display' => isset($options['display']) ? $options['display'] : array(
        'default' => array(
					'label' => 'hidden',
					'type' => 'hidden',
        ),
        'teaser' => array(
					'label' => 'hidden',
					'type' => 'hidden',
        ),
      ),
    );

    $instance = field_create_instance($instance);
  }
  return $instance;
}

/**
 * _ubercontent_get_field_values
 * A convenience method which simplifies the values from field arrays,
 * the original field values are stored in a new array [field_content]
 *
 * This processing occurs just before render so there shouldn't be any issues
 * effecting later proccessing caused by changing the fields
 *
 * @param $node
 * 	The node the get values for
 * @param $fields
 *  Array of fields to retrieve
 * @param $langcode
 * 	The language of the field to return
 */
function _ubercontent_get_field_values(&$node, $fields, $langcode = NULL) {
	global $language;
	if(!empty($node->field_content)) return; // already ran this function on this node, bail
	$langcode = !$langcode ? 'und' : $langcode;
	// setup the field_content array to store the old values
	$node->field_content = array();
	if(!$fields) return;
	foreach($fields as $field) {
		// get the field value for the given language
		$field_value = field_get_items('node', $node, $field, $langcode);
		// store the old field
		$node->field_content[$field] = $node->$field;
		// make sure und isn't involved
		if(isset($field_value['und'])) $field_value = $field_value['und'];
		// simplify the new node structure
		if(count($field_value) == 1) { // only one value present, just set the field value
			// for most field values, the [value] key will be set, otherwise just include the entire value array
			$node->$field = isset($field_value[0]['value']) ? $field_value[0]['value'] : $field_value[0];
		}
		else {
			$node->$field = array();
			foreach($field_value as $key => $item) { // multivalues are present, send back an array with each value numerically keyed
				// for most field values, the [value] key will be set, otherwise just include the entire value array
				$node->$field += isset($item['value']) ? array($key => $item['value']) : array($key => $item);
			}
		}
	}
}

/**
 * _ubercontent_get_options
 * get options from the options module
 * @param $field
 *		I'll need the field you're after, and it should probably be
 * 		a list_xxx field type
 */
function _ubercontent_get_options($field) {
	return _options_get_options(field_info_field($field), NULL, NULL, NULL, NULL);
}