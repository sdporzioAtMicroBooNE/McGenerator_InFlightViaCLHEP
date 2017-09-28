#include "Helper.h"

// Generate a single heavy sterile neutrino event
void GenerateObservables(CLHEP::HepRandomEngine& engine, twoIP_channel * CHAN, const FluxFile &flux, const Settings &set, Observables &obs)
{
	// Flat random provider
	CLHEP::RandFlat flat(engine);

	//The flux files don't provide phi or cos angles for the steriles, so we generate them here. Also set up positions and timings
	double	cosTh = 0.99999;
	double  phi = 2.0*M_PI*flat();
	double  energy  = flux.GetRandomEvent(engine);


	// Produce an initial heavy sterile neutrinos
	SterileNeutrino nus(set.sterileMass, energy, cosTh, phi);

	// Calling the appropriate functions from the channels.
	switch(CHAN->chan_identifier)
	{
		case CHAN_MUMU:
		CHAN->decayfunctionMassive(nus,MMU,MMU,0.0);
		break;
		case CHAN_MUE:
		CHAN->decayfunctionMassive(nus,MMU,ME,0.0);
		break;
		case CHAN_ELECPI:
		case CHAN_MUONPI:
		case CHAN_NUPI0:
		case CHAN_ELECPOSI:
		case CHAN_GAMMA:
		CHAN->decayfunction(nus);
		break;
	}

	// Fill the observables with the proper values from the decay
	CHAN->observables(&obs, engine);
	obs.E_sterile = nus.energy;
	obs.Th_sterile = nus.cosTh;
	// Calculate position and timing information
	double speedOfLight = 299792458.; // m/s
	double delay = set.distance/speedOfLight * (obs.E_sterile/(sqrt(pow(obs.E_sterile,2.) - pow(set.sterileMass,2.))) - 1.); // Delay with respect to active neutrinos

	double sterileDelay = delay*1e9; // Convert to ns
	double globalTimeOffset = set.globalTimeOffset;
	double randomTimeOffset = set.beamWindow*flat();

	obs.xPos = (236.35)*flat() + 10.;
	obs.yPos = (213)*flat() - 105.53;
	obs.zPos = (1016.8)*flat() + 10.1;
	obs.time = globalTimeOffset + randomTimeOffset + sterileDelay;

	return;
} // END Function GenerateObservables


// Fill model with theoretical parameters depending on the chosen decay channel.
void FillModel(CLHEP::HepRandomEngine& engine, twoIP_channel *&CHAN, std::vector<double> &model_params, const Settings &set)
{
	switch(set.decayChannel)
	{
		case CHAN_ELECPOSI:
		model_params.push_back(91.19); //mediator mass
		model_params.push_back((double) CHAN_ELECPOSI);
		model_params.push_back((double) CHAN_ELECPOSI);
		model_params.push_back((double) PDG_E);
		model_params.push_back(-1 * (double) PDG_E);
		CHAN = new threebody(engine,model_params);
		break;
		case CHAN_ELECPI:
		model_params.push_back(ME); // the electron with an incorrect mass (to try and avoid spacelike vectors...).
		model_params.push_back(MPION); // the pion. 0.135
		model_params.push_back((double) CHAN_ELECPI);
		model_params.push_back((double) PDG_E);
		model_params.push_back((double) PDG_PI);
		CHAN = new twobody(engine,model_params);
		break;
		case CHAN_MUONPI:
		model_params.push_back(MMU); // the muon.
		model_params.push_back(MPION); // the pion.
		model_params.push_back((double) CHAN_MUONPI);
		model_params.push_back((double) PDG_MU);
		model_params.push_back((double) PDG_PI);
		CHAN = new twobody(engine,model_params);
		break;
		case CHAN_NUPI0:
		model_params.push_back(0.00); // the neutrino
		model_params.push_back(MPI0); // the pion0
		model_params.push_back((double) CHAN_NUPI0);
		model_params.push_back((double) PDG_PI0);
		model_params.push_back(0.);
		CHAN = new twobody(engine,model_params);
		break;
		case CHAN_GAMMA:
		model_params.push_back(91.19); //mediator mass
		model_params.push_back((double) CHAN_GAMMA); // the pion.
		model_params.push_back((double) CHAN_GAMMA); // the pion.
		model_params.push_back((double) PDG_GAMMA);
		model_params.push_back(0.);
		CHAN = new threebody(engine,model_params);
		break;
		case CHAN_MUMU:
		model_params.push_back(91.19); //mediator mass
		model_params.push_back((double) CHAN_MUMU); // the pion.
		model_params.push_back((double) CHAN_MUMU); // the pion.
		model_params.push_back((double) PDG_MU);
		model_params.push_back(-1 * (double) PDG_MU);
		CHAN = new threebody(engine,model_params);
		break;
		case CHAN_MUE:
		model_params.push_back(91.19); //mediator mass
		model_params.push_back((double) CHAN_MUE); // the pion.
		model_params.push_back((double) CHAN_MUE); // the pion.
		model_params.push_back((double) PDG_MU);
		model_params.push_back(-1 * (double) PDG_E);
		CHAN = new threebody(engine,model_params);
		break;

		default:
		std::cout<<"ERROR: Bad channel specifier."<<std::endl;
		return;
	}

	return;
} // END function FillModel
