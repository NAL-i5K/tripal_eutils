<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class MegaTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  /**
   * @group mega
   */
  public function testAllProjects() {
   // $this->assertTrue(TRUE);
   // return;

    //Commentout the above line to run test.  Warning: this test will be slow!


    $projects = $this->getProjectList();

    foreach ($projects as $project) {

      tripal_eutils_create_records('bioproject', $project, TRUE);
      sleep(.5);
    }

    $this->assertTrue(TRUE);
  }


  private function getProjectList() {


    //Below projects were the child projects of NCBI project https://www.ncbi.nlm.nih.gov/bioproject/163973
    return [
      'PRJNA480452',
      'PRJNA427252',
      'PRJNA423280',
      'PRJNA423276',
      'PRJNA422877',
      'PRJNA420356',
      'PRJNA419349',
      'PRJNA348318',
      'PRJNA343475',
      'PRJNA342675',
      'PRJNA316108',
      'PRJNA298780',
      'PRJNA298750',
      'PRJNA297592',
      'PRJNA297581',
      'PRJNA282746',
      'PRJNA282653',
      'PRJNA278277',
      'PRJNA275759',
      'PRJNA275756',
      'PRJNA275754',
      'PRJNA275753',
      'PRJNA275750',
      'PRJNA275749',
      'PRJNA275741',
      'PRJNA275739',
      'PRJNA275666',
      'PRJNA275665',
      'PRJNA275664',
      'PRJNA275663',
      'PRJNA275662',
      'PRJNA275661',
      'PRJNA275660',
      'PRJNA275658',
      'PRJNA275657',
      'PRJNA275248',
      'PRJNA275247',
      'PRJNA275246',
      'PRJNA274806',
      'PRJNA271877',
      'PRJNA271706',
      'PRJNA243935',
      'PRJNA230921',
      'PRJNA229125',
      'PRJNA203545',
      'PRJNA203303',
      'PRJNA203301',
      'PRJNA203291',
      'PRJNA203209',
      'PRJNA203136',
      'PRJNA203089',
      'PRJNA203087',
      'PRJNA203045',
      'PRJNA198743',
      'PRJNA194433',
      'PRJNA171848',
      'PRJNA171756',
      'PRJNA171755',
      'PRJNA171753',
      'PRJNA171749',
      'PRJNA171748',
      'PRJNA168123',
      'PRJNA168121',
      'PRJNA168120',
      'PRJNA168119',
      'PRJNA168118',
      'PRJNA168116',
      'PRJNA167479',
      'PRJNA167478',
      'PRJNA167477',
      'PRJNA167476',
      'PRJNA167405',
      'PRJNA167404',
      'PRJNA167403',
      'PRJNA163993',
    ];
  }
}
