#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
//#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/L1TMuonEndCap/interface/Common.h"
#include "L1Trigger/L1TMuonEndCap/interface/EMTFSubsystemCollector.h"

#include <iostream>
#include <memory>

namespace {

  class MuonTriggerPrimitiveDumper : public edm::one::EDAnalyzer<> {
  public:
    explicit MuonTriggerPrimitiveDumper(edm::ParameterSet const&);
    ~MuonTriggerPrimitiveDumper() override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void beginJob() override {}
    void analyze(edm::Event const& iEvent, edm::EventSetup const&) override;
    void endJob() override {}

    // Tokens
    const edm::EDGetToken tokenCSC_;
    const edm::EDGetToken tokenRPC_;
    const edm::EDGetToken tokenGEM_;
    const edm::EDGetToken tokenME0_;
    //const edm::EDGetToken tokenDTPhi_;
    //const edm::EDGetToken tokenDTTheta_;

    // Member data
    std::unique_ptr<GeometryTranslator> tp_geom_;
  };

  // ___________________________________________________________________________
  MuonTriggerPrimitiveDumper::MuonTriggerPrimitiveDumper(const edm::ParameterSet& iConfig)
      : tokenCSC_(consumes<emtf::CSCTag::digi_collection>(iConfig.getParameter<edm::InputTag>("CSCInput"))),
        tokenRPC_(consumes<emtf::RPCTag::digi_collection>(iConfig.getParameter<edm::InputTag>("RPCInput"))),
        tokenGEM_(consumes<emtf::GEMTag::digi_collection>(iConfig.getParameter<edm::InputTag>("GEMInput"))),
        tokenME0_(consumes<emtf::ME0Tag::digi_collection>(iConfig.getParameter<edm::InputTag>("ME0Input"))),
        //tokenDTPhi_(consumes<emtf::DTTag::digi_collection>(iConfig.getParameter<edm::InputTag>("DTPhiInput"))),
        //tokenDTTheta_(consumes<emtf::DTTag::theta_digi_collection>(iConfig.getParameter<edm::InputTag>("DTThetaInput"))),
        tp_geom_(std::make_unique<GeometryTranslator>()) {}

  MuonTriggerPrimitiveDumper::~MuonTriggerPrimitiveDumper() {}

  void MuonTriggerPrimitiveDumper::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    // Extract all trigger primitives

    TriggerPrimitiveCollection muon_primitives;

    EMTFSubsystemCollector collector;

    bool useCSC = true;
    bool useRPC = false;
    bool useIRPC = false;
    bool useGEM = false;
    bool useME0 = false;
    bool useDT = false;

    if (useCSC) {
      collector.extractPrimitives(emtf::CSCTag(), iEvent, tokenCSC_, muon_primitives);
      //collector.extractPrimitives(emtf::CSCTag(), tp_geom_.get(), iEvent, tokenCSC_, muon_primitives);
    }
    if (useRPC) {
      collector.extractPrimitives(emtf::RPCTag(), iEvent, tokenRPC_, muon_primitives);
      //collector.extractPrimitives(emtf::RPCTag(), tp_geom_, iEvent, tokenRPC_, muon_primitives);
    }
    if (useIRPC) {
      collector.extractPrimitives(emtf::IRPCTag(), iEvent, tokenRPC_, muon_primitives);
      //collector.extractPrimitives(emtf::IRPCTag(), tp_geom_, iEvent, tokenRPC_, muon_primitives);
    }
    if (useGEM) {
      collector.extractPrimitives(emtf::GEMTag(), iEvent, tokenGEM_, muon_primitives);
      //collector.extractPrimitives(emtf::GEMTag(), tp_geom_, iEvent, tokenGEM_, muon_primitives);
    }
    if (useME0) {
      collector.extractPrimitives(emtf::ME0Tag(), iEvent, tokenME0_, muon_primitives);
      //collector.extractPrimitives(emtf::ME0Tag(), tp_geom_, iEvent, tokenME0_, muon_primitives);
    }
    if (useDT) {
      //collector.extractPrimitives(emtf::DTTag(), tp_geom_, iEvent, tokenDTPhi_, tokenDTTheta_, muon_primitives);
    }

    // Dump all trigger primitives

    std::cout << "# of TPs: " << muon_primitives.size() << std::endl;

    if (useCSC) {
      int i = 0;
      for (const auto& tp : muon_primitives) {
        if (tp.subsystem() == TriggerPrimitive::kCSC) {
          const CSCDetId& tp_detId = tp.detId<CSCDetId>();
          const CSCData& tp_data = tp.getCSCData();
          std::cout << "LCT #" << i++ << ": BX " << tp.getBX()
                    << " endcap " << tp_detId.endcap() << " sector " << tp_detId.triggerSector()
                    << " station " << tp_detId.station() << " ring " << tp_detId.ring()
                    << " chamber " << tp_detId.chamber() << " cscid " << tp_data.cscID
                    << " strip " << tp.getStrip() << " wire " << tp.getWire()
                    << " bend " << tp_data.bend << " qual " << tp_data.quality
                    << " pattern " << tp_data.pattern
                    << std::endl;
        }
      }  // end for loop
    }  // end if

    if (useRPC || useIRPC) {
      int i = 0;
      for (const auto& tp : muon_primitives) {
        if (tp.subsystem() == TriggerPrimitive::kRPC) {
          const RPCDetId& tp_detId = tp.detId<RPCDetId>();
          const RPCData& tp_data = tp.getRPCData();
          std::cout << "RPC #" << i++ << ": BX " << tp.getBX()
                    << " region " << tp_detId.region() << " sector_rpc " << tp_detId.sector()
                    << " station " << tp_detId.station() << " ring " << tp_detId.ring()
                    << " subsector_rpc " << tp_detId.subsector() << " roll " << tp_detId.roll()
                    << " strip " << tp.getStrip() << " (low) " << tp_data.strip_low << " (hi) " << tp_data.strip_hi
                    << " time " << tp_data.time << " valid " << tp_data.valid
                    << std::endl;
        }
      }  // end for loop
    }  // end if

    if (useGEM) {
      //int i = 0;
      //for (const auto& tp : muon_primitives) {
      //  if (tp.subsystem() == TriggerPrimitive::kGEM) {
      //    const GEMDetId& tp_detId = tp.detId<GEMDetId>();
      //    const GEMData& tp_data = tp.getGEMData();
      //    std::cout << "GEM #" << i++ << ": BX " << tp.getBX()
      //              << " region " << tp_detId.region() << " sector " << "X"
      //              << " station " << tp_detId.station() << " ring " << tp_detId.ring()
      //              << " chamber " << tp_detId.chamber() << " roll " << tp_detId.roll() << " layer " << tp_detId.layer()
      //              << " pad " << tp.getStrip() << " (low) " << tp_data.pad_low << " (hi) " << tp_data.pad_hi
      //              << std::endl;
      //  }
      //}  // end for loop
    }  // end if

    if (useME0) {
      //int i = 0;
      //for (const auto& tp : muon_primitives) {
      //  if (tp.subsystem() == TriggerPrimitive::kME0) {
      //    const ME0DetId& tp_detId = tp.detId<ME0DetId>();
      //    const ME0Data& tp_data = tp.getME0Data();
      //    std::cout << "ME0 #" << i++ << ": BX " << tp.getBX()
      //              << " region " << tp_detId.region() << " sector " << "X"
      //              << " station " << 1 << " ring " << 1
      //              << " chamber " << tp_detId.chamber() << " roll " << tp_detId.roll() << " layer " << tp_detId.layer()
      //              << " phiposition " << tp_data.phiposition << " deltaphi " << tp_data.deltaphi
      //              << " bend " << tp_data.bend << " quality " << tp_data.quality
      //              << " partition " << tp_data.partition
      //              << std::endl;
      //  }
      //}  // end for loop
    }  // end if

    if (useDT) {
      //int i = 0;
      //for (const auto& tp : muon_primitives) {
      //  if (tp.subsystem() == TriggerPrimitive::kDT) {
      //    const DTDetId& tp_detId = tp.detId<DTDetId>();
      //    const DTData& tp_data = tp.getDTData();
      //    std::cout << "DT #" << i++ << ": BX " << tp.getBX()
      //              << " wheel " << tp_detId.wheel() << " sector_dt " << tp_detId.sector()
      //              << " station " << tp_detId.station() << " bti_group " << tp_data.theta_bti_group
      //              << " strip " << tp.getStrip() << " wire " << tp.getWire()
      //              << " bend " << tp_data.bendingAngle << " quality " << tp_data.qualityCode
      //              << std::endl;
      //  }
      //}  // end for loop
    }  // end if

  }  // end function analyze()

  // ___________________________________________________________________________
  void MuonTriggerPrimitiveDumper::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("CSCInput", edm::InputTag("simCscTriggerPrimitiveDigis", "MPCSORTED"))
        ->setComment("InputTag for CSC trigger primitives");
    desc.add<edm::InputTag>("RPCInput", edm::InputTag("simMuonRPCDigis"))
        ->setComment("InputTag for RPC trigger primitives");
    desc.add<edm::InputTag>("GEMInput", edm::InputTag("simMuonGEMPadDigiClusters"))
        ->setComment("InputTag for GEM trigger primitives");
    desc.add<edm::InputTag>("ME0Input", edm::InputTag("me0TriggerConvertedPseudoDigis"))
        ->setComment("InputTag for ME0 trigger primitives");
    desc.add<edm::InputTag>("DTPhiInput", edm::InputTag("simTwinMuxDigis"))
        ->setComment("InputTag for DTPhi trigger primitives");
    desc.add<edm::InputTag>("DTThetaInput", edm::InputTag("simDtTriggerPrimitiveDigis"))
        ->setComment("InputTag for DTTheta trigger primitives");
    descriptions.add("muonTriggerPrimitiveDumper", desc);
  }

}  // anonymous namespace

// Define this as a plug-in
DEFINE_FWK_MODULE(MuonTriggerPrimitiveDumper);
