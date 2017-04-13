<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 12.04.17
 * Time: 17:15
 */

/**
 * This hook is called during the assembly notification (webhook)
 * of a Transloadit asynchronous request.
 *
 * @param $results
 *  An associative array containing the resulting files of a transloadit assembly response.
 * @param $data
 *  The complete data array of a transloadit assembly response.
 */
function hook_transloadit_api_notification($results, $data) {
  //
  // store the resulting files somewhere
}

/**
 * This hook can be used to alter a state that has been saved before the
 * asynchronous Transloadit request has been started.
 * This hook is only called, if the transloadit_api_execute_assembly() function is
 * called with a state.
 *
 * @param $state array
 *  The state variables to be altered during the asynchronous assembly notification.
 * @param $results
 *  An associative array containing the resulting files of a transloadit assembly.
 * @param $data
 *  The complete data array of a transloadit assembly response.
 *
 * @see transloadit_api_execute_assembly()
 */
function hook_transloadit_api_state_alter(&$state, $results, $data) {
  //
  // fill results into a node during asynchronous assembly notification
  $node = node_load($state['nid']);
  $node->converted_file_url[LANGUAGE_NONE][0]['url'] = $results['step_x']['file/path'];
  node_save($node);
}
