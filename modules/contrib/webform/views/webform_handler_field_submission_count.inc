<?php

/**
 * @file
 * Views handler to display the number of submissions in a webform.
 */

/**
 * Field handler to present the submission count of a node to the user.
 */
class webform_handler_field_submission_count extends views_handler_field {
  function construct() {
    parent::construct();
    $this->count_type = $this->definition['count_type'];

    if ($this->count_type == 'node') {
      $this->additional_fields['nid'] = 'nid';
      $this->additional_fields['type'] = 'type';
    }
    elseif ($this->count_type == 'users') {
      $this->additional_fields['uid'] = 'uid';
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['label'] = array('default' => '# of Submissions', 'translatable' => TRUE);
    return $options;
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $account = \Drupal::currentUser();

    $output = NULL;
    if ($this->count_type == 'node' && \Drupal::config('webform.settings')->get('node_' . $values->{$this->aliases['type']})) {
      module_load_include('inc', 'webform', 'includes/webform.submissions');
      $node = node_load($values->{$this->aliases['nid']});
      if (webform_results_access($node, $account)) {
        $count = webform_get_submission_count($node->id());
        $output = l($count, "node/$node->id()/webform-results");
      }
      elseif (webform_submission_access($node, NULL, 'list', $account)) {
        $count = webform_get_submission_count($node->id(), $account->id());
        $output = l($count, "node/$node->id()/submissions");
      }
      else {
        $output = webform_get_submission_count($node->id());
      }
    }
    elseif ($this->count_type == 'users') {
      $output = db_select('webform_submissions')
        ->condition('uid', $values->{$this->aliases['uid']})
        ->countQuery()->execute()->fetchField();
    }

    return $output;
  }
}
