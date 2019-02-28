<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;


class ImportFormTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group import_form
   */
  public function testAccessibilityToImportForm() {
    $this->actingAs(1);

    $response = $this->get('admin/tripal/loaders/eutils_loader');
    $response->assertSuccessful()
    ->assertSee('NCBI Accession Number');
  }

}
