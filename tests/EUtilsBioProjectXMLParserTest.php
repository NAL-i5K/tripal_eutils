<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsBioProjectXMLParserTest extends TripalTestCase {

  use DBTransaction;

  /**
   * @throws \Exception
   */
  public function testBioProject_submission_key() {
    $parser = new \EUtilsBioProjectParser();

    $submission_test_string = '<Submission last_update="2018-11-21" submission_id="SUB4827559" submitted="2018-11-21">
        <Description>
            <!-- Submitter information has been removed -->
            <Organization role="owner" type="institute">
                <Name>Christian Medical College</Name>
                <!-- Contact information has been removed -->
            </Organization>
            <Access>public</Access>
        </Description>
        <Action action_id="SUB4827559-3"/>
        <Action action_id="SUB4827559-bp0"/>
        <Action action_id="SUB4827559-bs0"/>
    </Submission>';

    $xml = simplexml_load_string($submission_test_string);

    $submission_info = $parser->bioProjectSubmission($xml);

    $this->assertNotEmpty($submission_info);
    $this->assertArrayHasKey('organization', $submission_info);
    $this->assertEquals('Christian Medical College',
      $submission_info['organization']);
  }

  /**
   * @test
   * @group project
   *
   */
  public function testBioProjectGeneralParsing() {

    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');
    foreach (glob("$path/examples/bioprojects/*.xml") as $file) {
      $parser = new \EUtilsBioProjectParser();
      $project = $parser->parse(simplexml_load_file($file));

      $this->assertArrayHasKey('name', $project);
      $this->assertArrayHasKey('accessions', $project);
      $this->assertArrayHasKey('attributes', $project);
      $this->assertArrayHasKey('description', $project);
      $this->assertArrayHasKey('linked_records', $project);

      $this->assertTrue(is_array($project['accessions']));
      $this->assertTrue(is_array($project['attributes']));

      $accessions = $project['accessions'];
      $props = $project['tagProps'];


      $linked_records = $project['linked_records'];

      $this->assertArrayHasKey('organism', $linked_records);

      /**
       * These will be used to lookup biosamples and assemblies.
       */
      if (isset($linked_records['locus_tag_prefix'])) {
        $locus_tag = $linked_records['locus_tag_prefix'];

        $this->assertArrayHasKey('value', $locus_tag);
        $this->assertArrayHasKey('attributes', $locus_tag);

      }

    }
  }

  /**
   * @group bioproject
   * @throws \Exception
   */
  public function testPubsParser(){

    $parser_master = new \EUtilsBioProjectParser();
    $parser = reflect($parser_master);


    $xml = simplexml_load_string($this->getProjectDscrXML());

    $parser->extractPubs($xml);


  }


  private function getProjectDscrXML(){

    return '<ProjectDescr>
            <Name>Bos mutus strain:yakQH1</Name>
            <Title>Bos mutus strain:yakQH1 RefSeq Genome</Title>
            <Description>The reference sequence (RefSeq) genome assembly is derived from the submitted GenBank assembly (see linked project PRJNA74739). Annotation provided on the RefSeq genomic records is based on NCBI annotation pipeline.</Description>
            <Publication id="9023104" status="ePublished">
                <Reference/>
                <DbType>ePubmed</DbType>
            </Publication>
            <Publication id="22751099" status="ePublished">
                <Reference/>
                <DbType>ePubmed</DbType>
            </Publication>
            <ProjectReleaseDate>2012-05-21T00:00:00Z</ProjectReleaseDate>
            <Relevance>
                <ModelOrganism>yes</ModelOrganism>
            </Relevance>
            <RefSeq representation="eReference">
                <AnnotationSource>
                    <Name>NCBI annotation pipeline</Name>
                </AnnotationSource>
            </RefSeq>
        </ProjectDescr>';

  }
}
