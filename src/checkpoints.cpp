// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
	typedef std::map<int, uint256> MapCheckpoints;

	// How many times we expect transactions after the last checkpoint to
	// be slower. This number is a compromise, as it can't be accurate for
	// every system. When reindexing from a fast disk with a slow CPU, it
	// can be up to 20, while when downloading from a slow network with a
	// fast multicore CPU, it won't be much higher than 1.
	static const double fSigcheckVerificationFactor = 5.0;

	struct CCheckpointData {
		const MapCheckpoints *mapCheckpoints;
		int64 nTimeLastCheckpoint;
		int64 nTransactionsLastCheckpoint;
		double fTransactionsPerDay;
	};

	// What makes a good checkpoint block?
	// + Is surrounded by blocks with reasonable timestamps
	//	 (no blocks before with a timestamp after, none after with
	//	  timestamp before)
	// + Contains no strange transactions
	static MapCheckpoints mapCheckpoints =
		boost::assign::map_list_of
		(	 4, uint256("0xfe508d41b7dc2c3079e827d4230e6f7ddebca43c9afc721c1e6431f78d6ff1de"))
		(	 5, uint256("0x7fc79021dbfa30255ade9bb8d898640516d9c771c3342a9b889ce380c52c6c1f"))
		( 5000, uint256("0xec268a9cfe87adb4d10a147a544493406c80e00a3e6868641e520b184e7ddce3"))
		(10000, uint256("0x29c63023c3b8a36b59837734a9c16133a4ef08d9a0e95f639a830c56e415070d"))
		(20000, uint256("0x3a0c072b76a298dabffc4f825a084c0f86dc55fe58f9bf31cc7e21bbfb2ead52"))
		(22500, uint256("0xfd3c87eae2e9be72499978155844598a8675eff7a61c90f9aebcedc94e1b217f"))
		(22544, uint256("0x6dd1a90cc56cf4a46c8c47528c4861c255e86d5f97fcee53ce356174e15c3045"))
		(22554, uint256("0xb13e8b128989f9a9fc1a4c1e547330d0b34d3f60189c00391a116922fa4fcb8c"))
		(22600, uint256("0x9e2d7f2fdab36c3e2b6f0455470cd957c12172ad7877f7c8e414fd736469c8d2"))
		(22650, uint256("0x7afbd354496346819b8a214693af70e1431bfadbf68d49a688ae27539fc6b37e"))
		(22700, uint256("0x35154b803fa5700b69f8081aa6d7c798c1e7fd027971252598a18549092a1291"))
		(22750, uint256("0x67e6eca7d46c1a612b7638e7a503e6dbc7cca4da493f4267833a6f1c9a655a35"))
		(22800, uint256("0x49e84c3b5c261966c37c101ac7691886bd641a382f514c2221735088b1b2beea"))
		(22850, uint256("0xc44cec57381a97c3983df0ef1fcf150669dd1794943202d89b805f423a65516f"))
		(22900, uint256("0x44de4c262de678a23554dd06a6f57270815ea9d145f6c542ab2a8dfbd2ca242c"))
		(22950, uint256("0xcecc4ab30b39fc09bf85eb191e64c1660ab2206c5f80953694997ec5c2db5338"))
		(25890, uint256("0x4806f91100ae83904aa0113cc3acda8fe6ac422186243719a68b76c98e7487c2"))
//		(23000, uint256("0x"))

		;
	static const CCheckpointData data = {
		&mapCheckpoints,
		1394847232, // * UNIX timestamp of last checkpoint block
		80219,		// * total number of transactions between genesis and last checkpoint
					//	 (the tx=... number in the SetBestChain debug.log lines)
		1000.0		// * estimated number of transactions per day after checkpoint
	};

	static MapCheckpoints mapCheckpointsTestnet = 
		boost::assign::map_list_of
		(	546, uint256("000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70"))
		( 35000, uint256("2af959ab4f12111ce947479bfcef16702485f04afd95210aa90fde7d1e4a64ad"))
		;
	static const CCheckpointData dataTestnet = {
		&mapCheckpointsTestnet,
		1369685559,
		37581,
		300
	};

	const CCheckpointData &Checkpoints() {
		if (fTestNet)
			return dataTestnet;
		else
			return data;
	}

	bool CheckBlock(int nHeight, const uint256& hash)
	{
		if (fTestNet) return true; // Testnet has no checkpoints
		if (!GetBoolArg("-checkpoints", true))
			return true;

		const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

		MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
		if (i == checkpoints.end()) return true;
		return hash == i->second;
	}

	// Guess how far we are in the verification process at the given block index
	double GuessVerificationProgress(CBlockIndex *pindex) {
		if (pindex==NULL)
			return 0.0;

		int64 nNow = time(NULL);

		double fWorkBefore = 0.0; // Amount of work done before pindex
		double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
		// Work is defined as: 1.0 per transaction before the last checkoint, and
		// fSigcheckVerificationFactor per transaction after.

		const CCheckpointData &data = Checkpoints();

		if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
			double nCheapBefore = pindex->nChainTx;
			double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
			double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
			fWorkBefore = nCheapBefore;
			fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
		} else {
			double nCheapBefore = data.nTransactionsLastCheckpoint;
			double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
			double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
			fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
			fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
		}

		return fWorkBefore / (fWorkBefore + fWorkAfter);
	}

	int GetTotalBlocksEstimate()
	{
		if (fTestNet) return 0; // Testnet has no checkpoints
		if (!GetBoolArg("-checkpoints", true))
			return 0;

		const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

		return checkpoints.rbegin()->first;
	}

	CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
	{
		if (fTestNet) return NULL; // Testnet has no checkpoints
		if (!GetBoolArg("-checkpoints", true))
			return NULL;

		const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

		BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
		{
			const uint256& hash = i.second;
			std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
			if (t != mapBlockIndex.end())
				return t->second;
		}
		return NULL;
	}
}
