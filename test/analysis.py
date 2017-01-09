#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt
import tables
from scipy import stats
import sys

"""
A python class to perform analysis of output of g4PBC test results

Usage: python ./analysis.py <particle_name> <number_primaries> <number_jobs>

"""

class Analysis:
    """   """
    #
    def __init__(self):
        self.particle_name = "geantino"
        self.number_modes = 4
        self.labels = ['semi-infinite world', 'finite world',
            'finite world (cyclic)', 'finite world (reflecting)']
        de = 0.02 # MeV
        self.e_bins = np.arange(0.0, 1.0+de, de) #ensure the final bin is considered
        dpz = 0.02 #
        self.pz_bins = np.arange(-1.0, 0.0, dpz) #ensure the final bin is considered
        self.number_jobs = 10
        self.number_primaries = 1000000
        self.n_total = np.float(self.number_jobs * self.number_primaries)
        self.cyc_mode = 0

    def read_hdf5_files(self, mode, njobs, quantity="direction_z"):
        """read all HDF5 files for this mode and particle type, fetch the quantity
        for each hit and collate results from all jobs"""
        res = None
        # only return rows satisfying particular particle type
        this_mask = "(particle_type==0)"# default is geantino
        if self.particle_name=='gamma':
            this_mask = "(particle_type==22)"
        elif self.particle_name=='e-':
            this_mask = "(particle_type==11)"
        elif self.particle_name=='e+':
            this_mask = "(particle_type==-11)"
        elif self.particle_name=='neutron':
            this_mask = "(particle_type==2112)"
        elif self.particle_name=='proton':
            this_mask = "(particle_type==2212)"
        #
        for i in np.arange(1,njobs+1):
            filename = "scorer_"+self.particle_name+"_"+np.str(mode)+"_"+\
                np.str(i)+".hdf5"
            table = tables.open_file(filename)
            vals = [x[quantity] for x in table.root.data.where(this_mask)]
            if res is None: res = np.array(vals)
            else: res = np.append(res,vals)
            table.close()
        return res

    def process_and_plot(self):
        """
        Creates histograms of kinetic energy and z-direction cosines at the
        scorer, and plots them.

        Computes the Kolmogorov-Smirnov
        statistic on 2 samples. This is a two-sided test for the null hypothesis
        that 2 independent samples are drawn from the same continuous distribution.

        The reference spectrum is mode 0 (semi-infinite planar)"""
        plt.figure(0)
        plt.figure(1)
        pzmax = -1
        kemax = -1
        pz_bin_centres = ((np.roll(an.pz_bins, -1) + an.pz_bins)/2.0)[:-1]
        e_bin_centres = ((np.roll(an.e_bins, -1) + an.e_bins)/2.0)[:-1]

        #we want all results on the same plot
        true_pzs = None
        true_kes = None
        for mode in np.arange(self.number_modes):
            pzs = self.read_hdf5_files(mode, self.number_jobs, "direction_z")
            kes = self.read_hdf5_files(mode, self.number_jobs, "kinetic_energy")
            #store the first samples as true values
            if true_pzs is None:
                true_kes = kes
                true_pzs = pzs
            else: #run the test
                ks_res = stats.ks_2samp(true_pzs, pzs)
                print("Result of KS test on ", self.particle_name, " mode ", mode)
                print("pzs: D statistic {0:5.2f} p stat {0:5.2f}".format(ks_res[0], ks_res[1]))
                ks_res = stats.ks_2samp(true_kes, kes)
                print("kes: D statistic {0:5.2f} p stat {0:5.2f}".format(ks_res[0], ks_res[1]))
                #TODO add logic to pass or fail results
            pz_hist, _ = np.histogram(pzs, bins=self.pz_bins)# []
            print("total counts pz ", len(pzs))
            pz_err = np.sqrt(pz_hist)
            #
            ke_hist, _ = np.histogram(kes, bins=self.e_bins)# []
            print("sanity check, total counts should be equal to 0.5 number primaries")
            print("total counts ke ", len(kes))
            ke_err = np.sqrt(ke_hist)
            #
            self.n_total = np.float(self.number_jobs * self.number_primaries)
            print("primaries per sim", self.number_primaries)
            print("total jobs", self.number_jobs)
            print("total primaries", self.n_total)
            #
            if pzmax < np.max(pz_hist) : pzmax = np.max(pz_hist)
            if kemax < np.max(ke_hist) : kemax = np.max(ke_hist)
            plt.figure(0)
            plt.errorbar(pz_bin_centres, pz_hist,
                yerr = pz_err,
                marker = '.',
                linestyle="-",
                drawstyle='steps-mid',
                label=self.labels[mode]
                )
            plt.figure(1)
            plt.errorbar(e_bin_centres, ke_hist,
                yerr = ke_err,
                linestyle="-",
                marker = '.',
                drawstyle='steps-mid',
                label=self.labels[mode]
                )
        plt.figure(0)
        plt.ylim(ymin=0, ymax=pzmax*1.5)
        plt.xlabel(r"$\cos \, \theta$")
        plt.legend(loc='upper right')
        plt.ylabel(r"$N$")
        plt.savefig(self.particle_name + "_pz_.png")
        plt.figure(1)
        plt.ylim(ymin=kemax/1e5, ymax=kemax*100)
        plt.xlabel(r"$K.E. \, MeV$")
        plt.ylabel(r"$N$")
        if self.particle_name == "proton":
            plt.legend(loc='upper left')
        else:
            plt.legend(loc='upper right')
        plt.semilogy()
        plt.savefig(self.particle_name + "_ke_.png")

# If run from standard input, script, or interactive prompt
if __name__ == "__main__":
    an = Analysis()
    an.particle_name = sys.argv[1]
    print("analysing results for ", an.particle_name)
    an.number_primaries = np.int(sys.argv[2])
    print("number primaries ", an.number_primaries)
    an.number_jobs = np.int(sys.argv[3])
    print("n modes ", an.number_modes)
    an.process_and_plot()
