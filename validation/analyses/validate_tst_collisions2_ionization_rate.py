import os, re, numpy as np
import happi

S = happi.Open(["./restart*"], verbose=False)



## Ionization cross section (code units) for Al3+ vs energy (keV)
#cs=np.array([
#	[0.001000,0.000000],[0.001177,0.000000],[0.001385,0.000000],[0.001630,0.000000],[0.001918,0.000000],[0.002257,0.000000],[0.002656,0.000000],[0.003126,0.000000],[0.003678,0.000000],[0.004329,0.000000],
#	[0.005094,0.000000],[0.005995,0.000000],[0.007055,0.000000],[0.008302,0.000000],[0.009770,0.000000],[0.011498,0.000000],[0.013531,0.000000],[0.015923,0.000000],[0.018738,0.000000],[0.022051,0.000000],
#	[0.025950,0.000000],[0.030539,0.000000],[0.035938,0.000000],[0.042293,0.000000],[0.049770,0.000000],[0.058570,0.000000],[0.068926,0.000000],[0.081113,0.000000],[0.095455,0.000000],[0.112333,0.000000],
#	[0.132194,0.008498],[0.155568,0.024568],[0.183074,0.042322],[0.215444,0.059764],[0.253537,0.074867],[0.298365,0.087020],[0.351120,0.096034],[0.413202,0.101988],[0.486261,0.105125],[0.572238,0.105786],
#	[0.673416,0.104358],[0.792484,0.101245],[0.932605,0.096836],[1.097501,0.091491],[1.291552,0.085529],[1.519914,0.079223],[1.788653,0.072813],[2.104908,0.066472],[2.477081,0.060321],[2.915059,0.054454],
#	[3.430476,0.048935],[4.037025,0.043802],[4.750819,0.039074],[5.590821,0.034755],[6.579345,0.030837],[7.742651,0.027303],[9.111645,0.024132],[10.72269,0.021300],[12.61859,0.018779],[14.84971,0.016545],
#	[17.47531,0.014570],[20.56516,0.012829],[24.20132,0.011297],[28.48041,0.009954],[33.51608,0.008778],[39.44213,0.007751],[46.41597,0.006855],[54.62287,0.006075],[64.28084,0.005398],[75.64647,0.004811],
#	[89.02167,0.004303],[104.7617,0.003865],[123.2848,0.003488],[145.0831,0.003164],[170.7355,0.002888],[200.9236,0.002653],[236.4493,0.002454],[278.2564,0.002288],[327.4554,0.002149],[385.3535,0.002035],
#	[453.4886,0.001944],[533.6708,0.001871],[628.0302,0.001815],[739.0734,0.001774],[869.7505,0.001746],[1023.532,0.001730],[1204.505,0.001723],[1417.476,0.001725],[1668.103,0.001735],[1963.044,0.001751],
#	[2310.133,0.001772],[2718.592,0.001798],[3199.272,0.001827],[3764.942,0.001860],[4430.628,0.001895],[5214.017,0.001932],[6135.917,0.001971],[7220.821,0.002012],[8497.548,0.002053],[10000.01,0.002096]
#])

for i in range(3):
	ion = "ion"+str(i)
	eon = "eon"+str(i)
	
	ion_mean_charge = S.ParticleBinning("#"+str(2*i)+"/#"+str(2*i+1),sum={"x":"all"}).get()
	times = ion_mean_charge["times"] * S.namelist.Main.timestep
	ion_mean_charge = np.array(ion_mean_charge["data"])
	
#	# theory
#	ion_charge   = S.namelist.Species[ion].charge
#	eon_density  = S.namelist.Species[eon].charge_density
#	eon_velocity = S.namelist.Species[eon].mean_velocity[0]
#	eon_energy   = ((1.-eon_velocity**2)**-.5-1.)*511. # energy in keV
#	eon_cs       = np.interp(np.log(eon_energy), np.log(cs[:,0]), cs[:,1]) # interpolate cross section
#	t = np.linspace(0., S.namelist.Main.simulation_time, 1000)
#	q = ion_charge + 1. - np.exp(-eon_velocity*eon_density*eon_cs*t)
#	plt.plot(t,q,'--k')
#	plt.plot(times, ion_mean_charge)
	
	Validate(ion+" mean charge", ion_mean_charge, 0.1)

