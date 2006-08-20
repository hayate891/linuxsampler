#include "GigWriteTest.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../gig.h"

CPPUNIT_TEST_SUITE_REGISTRATION(GigWriteTest);

using namespace std;

// file name of the Gigasampler file we are going to create for these tests
#define TEST_GIG_FILE_NAME "foo.gig"

// four stupid little sample "waves"
// (each having three sample points length, 16 bit depth, mono)
int16_t sampleData1[] = { 1, 2, 3 };
int16_t sampleData2[] = { 4, 5, 6 };
int16_t sampleData3[] = { 7, 8, 9 };
int16_t sampleData4[] = { 10,11,12 };

void GigWriteTest::printTestSuiteName() {
    cout << "\b \nTesting Gigasampler write support: " << flush;
}

// code executed when this test suite is created
void GigWriteTest::setUp() {
}

// code executed when this test suite will be destroyed
void GigWriteTest::tearDown() {
}


/////////////////////////////////////////////////////////////////////////////
// The actual test cases (in order) ...

// 1.) create a new Gigasampler file from scratch
void GigWriteTest::createNewGigFile() {
    try {
        // create an empty Gigasampler file
        gig::File file;
        // we give it an internal name, not mandatory though
        file.pInfo->Name = "Foo Gigasampler File";

        // create four samples
        gig::Sample* pSample1 = file.AddSample();
        gig::Sample* pSample2 = file.AddSample();
        gig::Sample* pSample3 = file.AddSample();
        gig::Sample* pSample4 = file.AddSample();
        // give those samples a name (not mandatory)
        pSample1->pInfo->Name = "Foo Sample 1";
        pSample2->pInfo->Name = "Foo Sample 2";
        pSample3->pInfo->Name = "Foo Sample 3";
        pSample4->pInfo->Name = "Foo Sample 4";
        // set meta informations for those samples
        pSample1->Channels = 1; // mono
        pSample1->BitDepth = 16; // 16 bits
        pSample1->FrameSize = 16/*bitdepth*/ / 8/*1 byte are 8 bits*/ * 1/*mono*/;
        pSample1->SamplesPerSecond = 44100;
        pSample2->Channels = 1; // mono
        pSample2->BitDepth = 16; // 16 bits
        pSample2->FrameSize = 16 / 8 * 1;
        pSample2->SamplesPerSecond = 44100;
        pSample3->Channels = 1; // mono
        pSample3->BitDepth = 16; // 16 bits
        pSample3->FrameSize = 16 / 8 * 1;
        pSample3->SamplesPerSecond = 44100;
        pSample4->Channels = 1; // mono
        pSample4->BitDepth = 16; // 16 bits
        pSample4->FrameSize = 16 / 8 * 1;
        pSample4->SamplesPerSecond = 44100;
        // resize those samples to a length of three sample points
        // (again: _sample_points_ NOT bytes!) which is the length of our
        // ficticious samples from above. after the Save() call below we can
        // then directly write our sample data to disk by using the Write()
        // method, that is without having to load all the sample data into
        // RAM. for large instruments / .gig files this is definitely the way
        // to go
        pSample1->Resize(3);
        pSample2->Resize(3);
        pSample3->Resize(3);
        pSample4->Resize(3);

        // create four instruments
        gig::Instrument* pInstrument1 = file.AddInstrument();
        gig::Instrument* pInstrument2 = file.AddInstrument();
        gig::Instrument* pInstrument3 = file.AddInstrument();
        gig::Instrument* pInstrument4 = file.AddInstrument();
        // give them a name (not mandatory)
        pInstrument1->pInfo->Name = "Foo Instrument 1";
        pInstrument2->pInfo->Name = "Foo Instrument 2";
        pInstrument3->pInfo->Name = "Foo Instrument 3";
        pInstrument4->pInfo->Name = "Foo Instrument 4";

        // create one region for each instrument
        // in this example we do not add a dimension, so
        // every region will have exactly one DimensionRegion
        // also we assign a sample to each dimension region
        gig::Region* pRegion = pInstrument1->AddRegion();
        pRegion->SetSample(pSample1);
        pRegion->pDimensionRegions[0]->pSample = pSample1;

        pRegion = pInstrument2->AddRegion();
        pRegion->SetSample(pSample2);
        pRegion->pDimensionRegions[0]->pSample = pSample2;

        pRegion = pInstrument3->AddRegion();
        pRegion->SetSample(pSample3);
        pRegion->pDimensionRegions[0]->pSample = pSample3;

        pRegion = pInstrument4->AddRegion();
        pRegion->SetSample(pSample4);
        pRegion->pDimensionRegions[0]->pSample = pSample4;

        // save file ("physically") as of now
        file.Save(TEST_GIG_FILE_NAME);
    } catch (RIFF::Exception e) {
        std::cerr << "\nCould not create a new Gigasampler file from scratch:\n" << std::flush;
        e.PrintMessage();
        throw e; // stop further tests
    }
}

// 2.) test if the newly created Gigasampler file exists & can be opened
void GigWriteTest::testOpenCreatedGigFile() {
    // try to open previously created Gigasampler file
    try {
        RIFF::File riff(TEST_GIG_FILE_NAME);
        gig::File file(&riff);
    } catch (RIFF::Exception e) {
        std::cerr << "\nCould not open newly created Gigasampler file:\n" << std::flush;
        e.PrintMessage();
        throw e; // stop further tests
    }
}

// 3.) test if the articulation informations of the newly created Gigasampler file were correctly written
void GigWriteTest::testArticulationsOfCreatedGigFile() {
    try {
        // open previously created Gigasampler file
        RIFF::File riff(TEST_GIG_FILE_NAME);
        gig::File file(&riff);
        // check amount of instruments and samples
        CPPUNIT_ASSERT(file.Instruments == 4);
        int iSamples = 0;
        for (gig::Sample* pSample = file.GetFirstSample(); pSample; pSample = file.GetNextSample()) iSamples++;
        CPPUNIT_ASSERT(iSamples == 4);

        //TODO: check all articulations previously set in createNewGigFile()

    } catch (RIFF::Exception e) {
        std::cerr << "\nThere was an exception while checking the articulation data of the newly created Gigasampler file:\n" << std::flush;
        e.PrintMessage();
        throw e; // stop further tests
    }
}

// 4.) try to write sample data to that newly created Gigasampler file
void GigWriteTest::testWriteSamples() {
    try {
        // open previously created Gigasampler file
        RIFF::File riff(TEST_GIG_FILE_NAME);
        gig::File file(&riff);
        // until this point we just wrote the articulation data to the .gig file
        // and prepared the .gig file for writing our 4 example sample data, so
        // now as the file exists physically and the 'samples' are already
        // of the correct size we can now write the actual samples' data by
        // directly writing them to disk
        gig::Sample* pSample1 = file.GetFirstSample();
        gig::Sample* pSample2 = file.GetNextSample();
        gig::Sample* pSample3 = file.GetNextSample();
        gig::Sample* pSample4 = file.GetNextSample();
        CPPUNIT_ASSERT(pSample1);
        pSample1->Write(sampleData1, 3);
        CPPUNIT_ASSERT(pSample2);
        pSample2->Write(sampleData2, 3);
        CPPUNIT_ASSERT(pSample3);
        pSample3->Write(sampleData3, 3);
        CPPUNIT_ASSERT(pSample4);
        pSample4->Write(sampleData4, 3);
    } catch (RIFF::Exception e) {
        std::cerr << "\nCould not directly write samples to newly created Gigasampler file:\n" << std::flush;
        e.PrintMessage();
        throw e; // stop further tests
    }
}
