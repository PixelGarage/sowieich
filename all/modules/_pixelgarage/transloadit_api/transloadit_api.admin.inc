<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 11.04.17
 * Time: 14:28
 */

/**
 * Stripe API admin settings form.
 */
function transloadit_api_admin_settings_form($form, $form_state) {
  $form['transloadit_api_links'] = array(
    '#markup' => t('!templates | !api_credentials | !docs<br /><br />', array(
      '!templates' => l(t('Templates'), 'https://transloadit.com/templates', array('attributes' => array('target' => '_blank'))),
      '!api_credentials' => l(t('API Credentials'), 'https://transloadit.com/accounts/credentials', array('attributes' => array('target' => '_blank'))),
      '!docs' => l(t('Docs'), 'https://transloadit.com/docs/api-docs/', array('attributes' => array('target' => '_blank'))),
    )),
  );

  //
  // Transloadit request
  $form['transloadit_api_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Transloadit Request Parameter'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 10,
  );
  $form['transloadit_api_auth_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Transloadit authentication key'),
    '#default_value' => variable_get('transloadit_api_auth_key', ''),
    '#fieldset' => 'transloadit_api_request',
  );
  $form['transloadit_api_auth_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Transloadit authentication secret'),
    '#default_value' => variable_get('transloadit_api_auth_secret', ''),
    '#fieldset' => 'transloadit_api_request',
  );
  $form['transloadit_api_localtunnel_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Localtunnel URL'),
    '#default_value' => variable_get('transloadit_api_localtunnel_url', ''),
    '#description' => t('The Localtunnel URL used for testing the webhook locally. If you have !localtunnel installed, copy the returned Localtunnel URL here to be automatically used, if Drupal runs on localhost and Localtunnel is installed.', array(
      '!localtunnel' => l(t('Localtunnel URL'), 'https://localtunnel.github.io/www/', array('attributes' => array('target' => '_blank'))),
    )),
    '#fieldset' => 'transloadit_api_request',
  );
  if (variable_get('transloadit_api_auth_secret', '')) {
    $form['transloadit_api_test'] = array(
      '#type' => 'button',
      '#value' => t('Test Transloadit Connection'),
      '#ajax' => array(
        'callback' => '_transloadit_api_admin_test_connection',
        'wrapper' => 'transloadit-connect-results',
        'method' => 'append',
      ),
      '#suffix' => '<div id="transloadit-connect-results"></div>',
      '#fieldset' => 'transloadit_api_request',
    );
  }

  if ($lt_url = variable_get('transloadit_api_localtunnel_url', false)) {
    $notification_url = $lt_url . '/transloadit/webhook';
  }
  else {
    $notification_url = url('transloadit/webhook', array('absolute' => TRUE));
  }
  $form['transloadit_api_webhook_url'] = array(
    '#type' => 'textfield',
    '#disabled' => TRUE,
    '#title' => t('Notification URL (webhook)'),
    '#default_value' => $notification_url,
    '#description' => t('The notification URL used for all transloadit asynchronous responses.'),
    '#fieldset' => 'transloadit_api_request',
  );

  //
  // Transloadit export ftp account
  $form['transloadit_api_ftp_account'] = array(
    '#type' => 'fieldset',
    '#title' => t('Transloadit FTP account'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('The FTP account is used by Transloadit to export the conversion results. For a localhost server (see Localtunnel above), no export step is performed. In this case the returned file URLs are valid for 24 hours.'),
    '#weight' => 20,
  );
  $form['transloadit_api_ftp_user'] = array(
    '#type' => 'textfield',
    '#title' => t('FTP user'),
    '#default_value' => $notification_url,
    '#description' => t('The FTP user used for the Transloadit file export step.'),
    '#fieldset' => 'transloadit_api_ftp_account',
  );
  $form['transloadit_api_ftp_password'] = array(
    '#type' => 'password_confirm',
    '#title' => t('FTP password'),
    '#default_value' => $notification_url,
    '#description' => t('The FTP password used for thenTransloadit file export step.'),
    '#fieldset' => 'transloadit_api_ftp_account',
  );


  return system_settings_form($form);
}

/**
 * AJAX callback to test the Transloadit connection.
 */
function _transloadit_api_admin_test_connection($form, $form_state) {
  $path = '/bill/' . date('Y-m');
  $request = transloadit_api_get_instance();
  $response = $request->request(array('method' => 'GET', 'path'   => $path), true);

  if ($response->data['ok'] === 'BILL_FOUND') {
    $you = $response->data['to'];
    $gb = $response->data['used_gb'];
    return t('Successful connection! @you used @gb GB this month.', array('@you' => $you, '@gb' => $gb));
  }
  else {
    return t('Error! @err', array('@err' => $response->error()));
  }
}
