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

    // Member data
    std::unique_ptr<GeometryTranslator> tp_geom_;
  };

  // ___________________________________________________________________________
  MuonTriggerPrimitiveDumper::MuonTriggerPrimitiveDumper(const edm::ParameterSet& iConfig)
      : tokenCSC_(consumes<emtf::CSCTag::digi_collection>(iConfig.getParameter<edm::InputTag>("CSCInput"))),
        tokenRPC_(consumes<emtf::RPCTag::digi_collection>(iConfig.getParameter<edm::InputTag>("RPCInput"))),
        tp_geom_(std::make_unique<GeometryTranslator>()) {}

  MuonTriggerPrimitiveDumper::~MuonTriggerPrimitiveDumper() {}

  void MuonTriggerPrimitiveDumper::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    // Extract all trigger primitives

    TriggerPrimitiveCollection muon_primitives;

    EMTFSubsystemCollector collector;

    bool useCSC = true;
    bool useRPC = false;

    if (useCSC) {
      collector.extractPrimitives(emtf::CSCTag(), iEvent, tokenCSC_, muon_primitives);
      //collector.extractPrimitives(emtf::CSCTag(), tp_geom_.get(), iEvent, tokenCSC_, muon_primitives);
    }

    if (useRPC) {
      collector.extractPrimitives(emtf::RPCTag(), iEvent, tokenRPC_, muon_primitives);
      //collector.extractPrimitives(emtf::RPCTag(), tp_geom_, iEvent, tokenRPC_, muon_primitives);
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

    if (useRPC) {
      int i = 0;
      for (const auto& tp : muon_primitives) {
        if (tp.subsystem() == TriggerPrimitive::kRPC) {
          const RPCDetId& tp_detId = tp.detId<RPCDetId>();
          const RPCData& tp_data = tp.getRPCData();
          std::cout << "RPC #" << i++ << ": BX " << tp.getBX()
                    << " endcap " << ((tp_detId.region() == -1) ? 2 : tp_detId.region()) << " sector_rpc " << tp_detId.sector()
                    << " station " << tp_detId.station() << " ring " << tp_detId.ring()
                    << " subsector_rpc " << tp_detId.subsector() << " roll " << tp_detId.roll()
                    << " strip " << tp.getStrip() << " (low) " << tp_data.strip_low << " (hi) " << tp_data.strip_hi
                    << " time " << tp_data.time << " valid " << tp_data.valid
                    << std::endl;
        }
      }  // end for loop
    }  // end if

  }  // end function analyze()

  // ___________________________________________________________________________
  void MuonTriggerPrimitiveDumper::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("CSCInput", edm::InputTag("simCscTriggerPrimitiveDigis", "MPCSORTED"))
        ->setComment("InputTag for CSC trigger primitives");
    desc.add<edm::InputTag>("RPCInput", edm::InputTag("simMuonRPCDigis"))
        ->setComment("InputTag for RPC trigger primitives");
    descriptions.add("muonTriggerPrimitiveDumper", desc);
  }

}  // anonymous namespace

// Define this as a plug-in
DEFINE_FWK_MODULE(MuonTriggerPrimitiveDumper);
