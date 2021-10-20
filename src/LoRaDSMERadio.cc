#include "LoRaDSMERadio.h"
#include "inet/physicallayer/wireless/common/medium/RadioMedium.h"
#include "inet/physicallayer/wireless/common/radio/packetlevel/Radio.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h"
#include "inet/common/Simsignals.h"

#define DUTY_CYCLE_PERIOD (3600U)

namespace inet {

namespace physicallayer {

Define_Module(LoRaDSMERadio);

void LoRaDSMERadio::initialize(int stage)
{
    LoRaRadio::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        dutyCycle = registerSignal("dutyCycle");
        dutyCycleTimer = new cMessage("dutyCycleTimer");
        scheduleAt(simTime() + SimTime(DUTY_CYCLE_PERIOD), dutyCycleTimer);
    }
}

void LoRaDSMERadio::aggregateToA(simtime_t T)
{
    aggregateTime += T;
}

void LoRaDSMERadio::endReception(cMessage *timer)
{
    auto part = (IRadioSignal::SignalPart)timer->getKind();
    auto signal = static_cast<WirelessSignal *>(timer->getControlInfo());
    auto arrival = signal->getArrival();
    auto reception = signal->getReception();

    if (!strcmp(timer->getName(), "receptionTimer") && isReceiverMode(radioMode) && arrival->getEndTime() == simTime()) {
        auto transmission = signal->getTransmission();
        // TODO: this would draw twice from the random number generator in isReceptionSuccessful: auto isReceptionSuccessful = medium->isReceptionSuccessful(this, transmission, part);
        auto isReceptionSuccessful = medium->getReceptionDecision(this, signal->getListening(), transmission, part)->isReceptionSuccessful();
        EV_INFO << "Reception ended: " << (isReceptionSuccessful ? "\x1b[1msuccessfully\x1b[0m" : "\x1b[1munsuccessfully\x1b[0m") << " for " << (IWirelessSignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
        auto macFrame = medium->receivePacket(this, signal);
        take(macFrame);
        decapsulate(macFrame);
        if (isReceptionSuccessful)
            sendUp(macFrame);
        else {
            emit(LoRaRadio::droppedPacket, 0);
            delete macFrame;
        }
        receptionTimer = nullptr;
        emit(receptionEndedSignal, check_and_cast<const cObject *>(reception));
    }
    else
        EV_INFO << "Reception ended: \x1b[1mignoring\x1b[0m " << (IWirelessSignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
    updateTransceiverState();
    updateTransceiverPart();
    delete timer;
    // TODO: move to radio medium
    check_and_cast<RadioMedium *>(medium)->emit(IRadioMedium::signalArrivalEndedSignal, check_and_cast<const cObject *>(reception));
/*
    auto part = (IRadioSignal::SignalPart)timer->getKind();
    auto signal = static_cast<Signal *>(timer->getControlInfo());
    auto arrival = signal->getArrival();
    auto reception = signal->getReception();
    if (timer == receptionTimer && isReceiverMode(radioMode) && arrival->getEndTime() == simTime()) {
    //if (isReceiverMode(radioMode) && arrival->getEndTime() == simTime()) {
        auto transmission = signal->getTransmission();
// TODO: this would draw twice from the random number generator in isReceptionSuccessful: auto isReceptionSuccessful = medium->isReceptionSuccessful(this, transmission, part);
        auto isReceptionSuccessful = medium->getReceptionDecision(this, signal->getListening(), transmission, part)->isReceptionSuccessful();
        EV_INFO << "Reception ended: " << (isReceptionSuccessful ? "successfully" : "unsuccessfully") << " for " << (ISignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
        auto macFrame = medium->receivePacket(this, signal);
        auto tag = macFrame->addTag<lora::LoRaTag>();
        auto preamble = macFrame->popAtFront<LoRaPhyPreamble>();
        tag->setBandwidth(preamble->getBandwidth());
        tag->setCenterFrequency(preamble->getCenterFrequency());
        tag->setCodeRendundance(preamble->getCodeRendundance());
        tag->setPower(preamble->getPower());
        tag->setSpreadFactor(preamble->getSpreadFactor());
        tag->setUseHeader(preamble->getUseHeader());
        if(isReceptionSuccessful) {
            emit(packetSentToUpperSignal, macFrame);
            sendUp(macFrame);
        }
        else {
            emit(LoRaRadio::droppedPacket, 0);
            delete macFrame;
        }
        emit(receptionEndedSignal, check_and_cast<const cObject *>(reception));
        receptionTimer = nullptr;
    }
    else
        EV_INFO << "Reception ended: ignoring " << (ISignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
    updateTransceiverState();
    updateTransceiverPart();
    //check_and_cast<LoRaMedium *>(medium)->fireReceptionEnded(reception);
    //check_and_cast<RadioMedium *>(medium)->emit(receptionEndedSignal, check_and_cast<const cObject *>(reception));
    check_and_cast<LoRaMedium *>(medium)->emit(IRadioMedium::signalArrivalEndedSignal, check_and_cast<const cObject *>(reception));
    delete timer;
    */
}
void LoRaDSMERadio::handleSelfMessage(cMessage *message)
{
    if(message == dutyCycleTimer) {
        scheduleAt(simTime() + DUTY_CYCLE_PERIOD, dutyCycleTimer);

        simtime_t dc = aggregateTime * 100 / SimTime(3600);
        emit(dutyCycle, dc);
        aggregateTime = 0;
    }
    else {
        LoRaRadio::handleSelfMessage(message);
    }
}

}
}
