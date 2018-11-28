<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EutilsXmlParserTest extends TripalTestCase{

  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testInit() {
    $parser = new \EutilsXMLParser('bioproject');
    $this->assertNotNull($parser);

    $connection = new \EUtils();

    //$connection->setDB('bioproject');
    //https://www.ncbi.nlm.nih.gov/bioproject/PRJNA506315
    $result = $connection->lookupAccessions('bioproject', ['506315']);

    $parser->loadXML($result);
  }

  public function testParserBasicsBioProject() {
    $parser = new \EutilsXMLParser('bioproject');
    $xml = simplexml_load_file(__DIR__ . '/example_files/example_pertussis.xml');
    $parser->loadXML($xml);
  }

  /**
   * @throws \Exception
   */
  public function testBioProject_submission_key() {
    $parser = new \EutilsXMLParser('bioproject');

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

    $parser_r = reflect($parser);
    $submission_info = $parser_r->bioprojectSubmission($xml);

    $this->assertNotEmpty($submission_info);
    $this->assertArrayHasKey('organization', $submission_info);
    $this->assertEquals('Christian Medical College',
      $submission_info['organization']);
  }

  /**
   * @group wip
   */
  public function testBioproject_attributes_parser() {
    $parser = new \EutilsXMLParser('bioproject');

    $string = ' <Project>
        <ProjectID>
            <ArchiveID accession="PRJNA506315" archive="NCBI" id="506315"/>
            <LocalID>bp0</LocalID>
            <LocalID>bp0</LocalID>
        </ProjectID>
        <ProjectDescr>
            <Name>Bordetella pertussis strain:BPD2</Name>
            <Title>Bordetella pertussis strain:BPD2 Genome sequencing</Title>
            <Description>Complete genome sequence of Bordetella pertussis</Description>
            <LocusTagPrefix biosample_id="SAMN10457990">EHO96</LocusTagPrefix>
        </ProjectDescr>
        <ProjectType>
            <ProjectTypeSubmission>
                <Target capture="eWhole" material="eGenome" sample_scope="eMonoisolate">
                    <Organism species="520" taxID="520">
                        <OrganismName>Bordetella pertussis</OrganismName>
                        <Strain>BPD2</Strain>
                        <Supergroup>eBacteria</Supergroup>
                    </Organism>
                </Target>
                <Method method_type="eSequencing"/>
                <Objectives>
                    <Data data_type="eSequence"/>
                </Objectives>
                <IntendedDataTypeSet>
                    <DataType>genome sequencing</DataType>
                </IntendedDataTypeSet>
                <ProjectDataTypeSet>
                    <DataType>genome sequencing</DataType>
                </ProjectDataTypeSet>
            </ProjectTypeSubmission>
        </ProjectType>
    </Project>';

    $xml = simplexml_load_string($string);

    $parser_r = reflect($parser);
    $submission_info = $parser_r->bioprojectProject($xml);

    $this->assertNotEmpty($submission_info);

    $string = '<Project>
        <ProjectID>
        </ProjectID>
        <ProjectDescr>        
        </ProjectDescr>
        <ProjectType>         
        </ProjectType>
        <Waffle>I should cause an exception</Waffle>
    </Project>';

    $xml = simplexml_load_string($string);

    $this->expectException('Exception');
    $parser_r = reflect($parser);
    $submission_info = $parser_r->bioprojectProject($xml);
    $this->assertFalse($submission_info);
  }
}
