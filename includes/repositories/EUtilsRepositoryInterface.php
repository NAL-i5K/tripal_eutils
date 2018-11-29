<?php

abstract class EUtilsRepositoryInterface{

  /**
   * List of required fields.
   *
   * @var array
   */
  protected $required_fields = [];

  /**
   * Create a new resource.
   *
   * @param array $data
   *
   * @return object
   */
  abstract public function create($data);

  /**
   * Determine whether required fields are provided.
   *
   * @param array $data
   * @throws \Exception
   */
  public function validateFields($data) {
    foreach ($this->required_fields as $field) {
      if (!isset($data[$field])) {
        $class_name = get_class($this);
        throw new Exception('Required field ' . $field . ' is missing in ' . $class_name);
      }
    }
  }
}
