<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 16.04.17
 * Time: 16:34
 */

/* =============================================================================
 *   Transloadit API methods
 * ========================================================================== */
/**
 * Returns an instance of a \transloadit\Transloadit object with the key and
 * secret set, or false, if an error occurred.
 *
 * @return bool|\transloadit\Transloadit
 */
function transloadit_api_get_instance() {
  $library = libraries_load('transloadit');
  if ($library['loaded']) {
    $transloadit = new \transloadit\Transloadit(array(
      'key' => variable_get('transloadit_api_auth_key'),
      'secret' => variable_get('transloadit_api_auth_secret'),
    ));
    return $transloadit;
  }

  watchdog('Transloadit API', 'The Transloadit library is not loaded.', array(), WATCHDOG_ERROR);
  return FALSE;
}

/**
 * Executes the given template or assembly steps on all given files and returns
 * the \transloadit\TransloaditResponse object. The file conversion
 * (transcoding) can be performed asynchronously or synchronously.
 *
 * In case of an asynchronous execution, the result of the conversion
 * (transcoding) is returned in the assembly notification callback (see API
 * functions).
 *
 * @param array  $files
 *     An associative array of files to be uploaded. The key is used in the
 *     response as identification of the file.
 * @param string $template_id
 *     The template ID of a defined template in your transloadit account. Set
 *     it to null or an empty string if the default template should be used.
 * @param array  $steps
 *     An associative array of transloadit assembly steps. In case of a given
 *     template ID the steps are merged (overridden) with the template steps.
 *     Default = array().
 * @param array  $context
 *     An associative array holding the context of the transloadit conversion,
 *     e.g. a node id of the node that stores the converted files.
 *     This $context array is available in the response of an asynchronous
 *     assembly execution.
 * @param bool   $async
 *     True, if the assembly is executed asynchronously, false otherwise.
 *     Default = true.
 *
 * @return bool|null|string|\transloadit\TransloaditResponse
 *  Returns false in case of an error, otherwise the
 *  \transloadit\TransloaditResponse object.
 */
function transloadit_api_execute_assembly($files, $template_id = '', $steps = array(), $context = array(), $async = TRUE) {
  //
  // create params array
  $params = array();
  $template_id = !empty($template_id) ? $template_id : variable_get('transloadit_api_template_id', '');
  if (!empty($template_id)) {
    $params['template_id'] = $template_id;
  }
  else if (empty($steps)) {
    watchdog('Transloadit API', 'No template or assembly steps are defined.', array(), WATCHDOG_DEBUG);
    return false;
  }
  if (!empty($steps)) {
    $params['steps'] = $steps;
  }
  if ($async) {
    if ($lt_url = variable_get('transloadit_api_localtunnel_url', false)) {
      $ide_key = $_COOKIE['XDEBUG_SESSION'];
      $debug_param = $ide_key ? '?XDEBUG_SESSION_START=' . $ide_key : '';
      $params['notify_url'] = $lt_url . '/' . TRANSLOADIT_NOTIFY_WEBHOOK . $debug_param;
    }
    else {
      $params['notify_url'] = url(TRANSLOADIT_NOTIFY_WEBHOOK, array('absolute' => TRUE));
    }
  }

  // create assembly options
  $options = array(
    'files' => $files,
    'params' => $params,
  );

  //
  // execute assembly
  if ($transloadit = transloadit_api_get_instance()) {
    $response = $transloadit->createAssembly($options);
    if (isset($response->data['error'])) {
      watchdog('Transloadit API', 'Error @code: @msg', array(
        '@code' => $response->data['error'],
        '@msg' => $response->data['message']
      ), WATCHDOG_ERROR);
      return FALSE;
    }
    else {
      if (isset($response->data['ok'])) {
        watchdog('Transloadit API', 'Status @code: @msg', array(
          '@code' => $response->data['ok'],
          '@msg' => $response->data['message']
        ), WATCHDOG_DEBUG);
      }
    }

    //
    // save the context for the assembly
    $assembly_id = $response->data['assembly_id'];
    variable_set('transloadit_api_context_' . $assembly_id, $context);

    return $response;
  }
  else {
    return FALSE;
  }
}
