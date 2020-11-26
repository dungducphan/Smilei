#include "ElectroMagnBC3D_SM.h"

#include <cstdlib>

#include <iostream>
#include <string>

#include "Params.h"
#include "Patch.h"
#include "ElectroMagn.h"
#include "Field3D.h"
#include "Tools.h"
#include "Laser.h"

using namespace std;

ElectroMagnBC3D_SM::ElectroMagnBC3D_SM( Params &params, Patch *patch, unsigned int _min_max )
    : ElectroMagnBC3D( params, patch, _min_max )
{

    std::vector<unsigned int> dims( 2, 0 );
    
    Bx_val = By_val = Bz_val = nullptr;
    
    if( min_max==0 && patch->isXmin() ) {
        // BCs at the x-border min
        dims = { ny_d, nz_d }; // Bx^(p,d,d)
        Bx_val = new Field2D( dims, "Bx_val" );
        Bx_val->put_to( 0. );
        dims = { ny_p, nz_d }; // By^(d,p,d)
        By_val = new Field2D( dims, "By_val" );
        By_val->put_to( 0. );
        dims = { ny_d, nz_p }; // Bz^(d,d,p)
        Bz_val = new Field2D( dims, "Bz_val" );
        Bz_val->put_to( 0. );
    } else if( min_max==1 && patch->isXmax() ) {
        // BCs at the x-border max
        dims = { ny_d, nz_d }; // Bx^(p,d,d)
        Bx_val = new Field2D( dims, "Bx_val" );
        Bx_val->put_to( 0. );
        dims = { ny_p, nz_d }; // By^(d,p,d)
        By_val = new Field2D( dims, "By_val" );
        By_val->put_to( 0. );
        dims = { ny_d, nz_p }; // Bz^(d,d,p)
        Bz_val = new Field2D( dims, "Bz_val" );
        Bz_val->put_to( 0. );
    } else if( min_max==2 && patch->isYmin() ) {
        // BCs in the y-border min
        dims = { nx_p, nz_d }; // Bx^(p,d,d)
        Bx_val = new Field2D( dims, "Bx_val" );
        Bx_val->put_to( 0. );
        dims = { nx_d, nz_d }; // By^(d,p,d)
        By_val = new Field2D( dims, "By_val" );
        By_val->put_to( 0. );
        dims = { nx_d, nz_p }; // Bz^(d,d,p)
        Bz_val = new Field2D( dims, "Bz_val" );
        Bz_val->put_to( 0. );
    } else if( min_max==3 && patch->isYmax() ) {
        // BCs in the y-border mix
        dims = { nx_p, nz_d }; // Bx^(p,d,d)
        Bx_val = new Field2D( dims, "Bx_val" );
        Bx_val->put_to( 0. );
        dims = { nx_d, nz_d }; // By^(d,p,d)
        By_val = new Field2D( dims, "By_val" );
        By_val->put_to( 0. );
        dims = { nx_d, nz_p }; // Bz^(d,d,p)
        Bz_val = new Field2D( dims, "Bz_val" );
        Bz_val->put_to( 0. );
    } else if( min_max==4 && patch->isZmin() ) {
        // BCs in the z-border min
        dims = { nx_p, ny_d }; // Bx^(p,d,d)
        Bx_val = new Field2D( dims, "Bx_val" );
        Bx_val->put_to( 0. );
        dims = { nx_d, ny_p }; // By^(d,p,d)
        By_val = new Field2D( dims, "By_val" );
        By_val->put_to( 0. );
        dims = { nx_d, ny_d }; // Bz^(d,d,p)
        Bz_val = new Field2D( dims, "Bz_val" );
        Bz_val->put_to( 0. );
    } else if( min_max==5 && patch->isZmax() ) {
        // BCs in the z-border max
        dims = { nx_p, ny_d }; // Bx^(p,d,d)
        Bx_val = new Field2D( dims, "Bx_val" );
        Bx_val->put_to( 0. );
        dims = { nx_d, ny_p }; // By^(d,p,d)
        By_val = new Field2D( dims, "By_val" );
        By_val->put_to( 0. );
        dims = { nx_d, ny_d }; // Bz^(d,d,p)
        Bz_val = new Field2D( dims, "Bz_val" );
        Bz_val->put_to( 0. );
    }
    
    
    // -----------------------------------------------------
    // Parameters for the Silver-Mueller boundary conditions
    // -----------------------------------------------------
    
    //! \todo (MG) Check optimal angle for Silver-Muller BCs
    double pyKx, pyKy, pyKz;
    double kx, ky, kz;
    double Knorm;
    double omega = 1. ;
    //kx = w cos(theta) cos(phi)
    //ky = w sin(theta)
    //kz = w cos(theta) sin(phi)
    
    // Xmin boundary
    pyKx = params.EM_BCs_k[0][0];
    pyKy = params.EM_BCs_k[0][1];
    pyKz = params.EM_BCs_k[0][2];
    Knorm = sqrt( pyKx*pyKx + pyKy*pyKy + pyKz*pyKz ) ;
    kx = omega*pyKx/Knorm;
    ky = omega*pyKy/Knorm;
    kz = omega*pyKz/Knorm;
    
    double factor = 1.0 / ( kx + dt_ov_dx );
    Alpha_SM_W    = 2.0                     * factor;
    Beta_SM_W     = - ( kx-dt_ov_dx ) * factor;
    Gamma_SM_W    = 4.0 * kx        * factor;
    Delta_SM_W    = - ( ky + dt_ov_dy ) * factor;
    Epsilon_SM_W  = - ( ky - dt_ov_dy ) * factor;
    Zeta_SM_W     = - ( kz + dt_ov_dz ) * factor;
    Eta_SM_W      = - ( kz - dt_ov_dz ) * factor;
    
    // Xmax boundary
    pyKx = params.EM_BCs_k[1][0];
    pyKy = params.EM_BCs_k[1][1];
    pyKz = params.EM_BCs_k[1][2];
    Knorm = sqrt( pyKx*pyKx + pyKy*pyKy + pyKz*pyKz ) ;
    kx = omega*pyKx/Knorm;
    ky = omega*pyKy/Knorm;
    kz = omega*pyKz/Knorm;
    
    factor        = 1.0 / ( kx - dt_ov_dx );
    Alpha_SM_E    = 2.0                      * factor;
    Beta_SM_E     = - ( kx+dt_ov_dx )  * factor;
    Gamma_SM_E    = 4.0 * kx         * factor;
    Delta_SM_E    = - ( ky + dt_ov_dy )  * factor;
    Epsilon_SM_E  = - ( ky - dt_ov_dy )  * factor;
    Zeta_SM_E     = - ( kz + dt_ov_dz ) * factor;
    Eta_SM_E      = - ( kz - dt_ov_dz ) * factor;
    
    // Ymin boundary
    pyKx = params.EM_BCs_k[2][0];
    pyKy = params.EM_BCs_k[2][1];
    pyKz = params.EM_BCs_k[2][2];
    Knorm = sqrt( pyKx*pyKx + pyKy*pyKy + pyKz*pyKz ) ;
    kx = omega*pyKx/Knorm;
    ky = omega*pyKy/Knorm;
    kz = omega*pyKz/Knorm;
    
    factor = 1.0 / ( ky + dt_ov_dy );
    Alpha_SM_S    = 2.0                     * factor;
    Beta_SM_S     = - ( ky - dt_ov_dy ) * factor;
    Delta_SM_S    = - ( kz + dt_ov_dz ) * factor;
    Epsilon_SM_S  = - ( kz -dt_ov_dz ) * factor;
    Zeta_SM_S     = - ( kx + dt_ov_dx ) * factor;
    Eta_SM_S      = - ( kx - dt_ov_dx ) * factor;
    
    // Ymax boundary
    pyKx = params.EM_BCs_k[3][0];
    pyKy = params.EM_BCs_k[3][1];
    pyKz = params.EM_BCs_k[3][2];
    Knorm = sqrt( pyKx*pyKx + pyKy*pyKy + pyKz*pyKz ) ;
    kx = omega*pyKx/Knorm;
    ky = omega*pyKy/Knorm;
    kz = omega*pyKz/Knorm;
    
    factor = 1.0 / ( ky - dt_ov_dy );
    Alpha_SM_N    = 2.0                     * factor;
    Beta_SM_N     = - ( ky + dt_ov_dy ) * factor;
    Delta_SM_N    = - ( kz + dt_ov_dz ) * factor;
    Epsilon_SM_N  = - ( kz - dt_ov_dz ) * factor;
    Zeta_SM_N     = - ( kx + dt_ov_dx ) * factor;
    Eta_SM_N      = - ( kx - dt_ov_dx ) * factor;
    
    // Zmin boundary
    pyKx = params.EM_BCs_k[4][0];
    pyKy = params.EM_BCs_k[4][1];
    pyKz = params.EM_BCs_k[4][2];
    Knorm = sqrt( pyKx*pyKx + pyKy*pyKy + pyKz*pyKz ) ;
    kx = omega*pyKx/Knorm;
    ky = omega*pyKy/Knorm;
    kz = omega*pyKz/Knorm;
    
    factor = 1.0 / ( kz + dt_ov_dz );
    Alpha_SM_B    = 2.0                     * factor;
    Beta_SM_B     = - ( kz - dt_ov_dz ) * factor;
    Delta_SM_B    = - ( kx + dt_ov_dx ) * factor;
    Epsilon_SM_B  = - ( kx - dt_ov_dx ) * factor;
    Zeta_SM_B     = - ( ky + dt_ov_dy ) * factor;
    Eta_SM_B      = - ( ky - dt_ov_dy ) * factor;
    
    // Zmax boundary
    pyKx = params.EM_BCs_k[5][0];
    pyKy = params.EM_BCs_k[5][1];
    pyKz = params.EM_BCs_k[5][2];
    Knorm = sqrt( pyKx*pyKx + pyKy*pyKy + pyKz*pyKz ) ;
    kx = omega*pyKx/Knorm;
    ky = omega*pyKy/Knorm;
    kz = omega*pyKz/Knorm;
    
    factor        = 1.0 / ( kz - dt_ov_dz );
    Alpha_SM_T    = 2.0                      * factor;
    Beta_SM_T     = - ( kz + dt_ov_dz )  * factor;
    Delta_SM_T    = - ( kx + dt_ov_dx )  * factor;
    Epsilon_SM_T  = - ( kx - dt_ov_dx )  * factor;
    Zeta_SM_T     = - ( ky + dt_ov_dy ) * factor;
    Eta_SM_T      = - ( ky - dt_ov_dy ) * factor;
    
}

ElectroMagnBC3D_SM::~ElectroMagnBC3D_SM()
{
    if( Bx_val ) {
        delete Bx_val ;
    }
    if( By_val ) {
        delete By_val ;
    }
    if( Bz_val ) {
        delete Bz_val ;
    }
}


// Magnetic field Bx^(p,d,d)
// Magnetic field By^(d,p,d)
// Magnetic field Bz^(d,d,p)

void ElectroMagnBC3D_SM::save_fields( Field *my_field, Patch *patch )
{
    Field3D *field3D=static_cast<Field3D *>( my_field );
    
    if( min_max==0 && patch->isXmin() ) {
    
        if( field3D->name=="Bx" ) {
            field3D->extract_slice_yz( 0,      Bx_val );
        } else if( field3D->name=="By" ) {
            field3D->extract_slice_yz( 0,      By_val );
        } else if( field3D->name=="Bz" ) {
            field3D->extract_slice_yz( 0,      Bz_val );
        }
    } else if( min_max==1 && patch->isXmax() ) {
        if( field3D->name=="Bx" ) {
            field3D->extract_slice_yz( 0,      Bx_val );
        } else if( field3D->name=="By" ) {
            field3D->extract_slice_yz( 0,      By_val );
        } else if( field3D->name=="Bz" ) {
            field3D->extract_slice_yz( 0,      Bz_val );
        }
    } else if( min_max==2 && patch->isYmin() ) {
        if( field3D->name=="Bx" ) {
            field3D->extract_slice_xz( 0,      Bx_val );
        } else if( field3D->name=="By" ) {
            field3D->extract_slice_xz( 0,      By_val );
        } else if( field3D->name=="Bz" ) {
            field3D->extract_slice_xz( 0,      Bz_val );
        }
    } else if( min_max==3 && patch->isYmax() ) {
        if( field3D->name=="Bx" ) {
            field3D->extract_slice_xz( ny_d-1, Bx_val );
        } else if( field3D->name=="By" ) {
            field3D->extract_slice_xz( ny_p-1, By_val );
        } else if( field3D->name=="Bz" ) {
            field3D->extract_slice_xz( ny_d-1, Bz_val );
        }
    } else if( min_max==4 && patch->isZmin() ) {
    
        if( field3D->name=="Bx" ) {
            field3D->extract_slice_xy( 0,      Bx_val );
        } else if( field3D->name=="By" ) {
            field3D->extract_slice_xy( 0,      By_val );
        } else if( field3D->name=="Bz" ) {
            field3D->extract_slice_xy( 0,      Bz_val );
        }
    } else if( min_max==5 && patch->isZmax() ) {
    
        if( field3D->name=="Bx" ) {
            field3D->extract_slice_xy( nz_d-1, Bx_val );
        } else if( field3D->name=="By" ) {
            field3D->extract_slice_xy( nz_d-1, By_val );
        } else if( field3D->name=="Bz" ) {
            field3D->extract_slice_xy( nz_p-1, Bz_val );
        }
    }
}


void ElectroMagnBC3D_SM::disableExternalFields()
{
    delete Bx_val;
    Bx_val = NULL;
    delete By_val;
    By_val = NULL;
    delete Bz_val;
    Bz_val = NULL;
}


// ---------------------------------------------------------------------------------------------------------------------
// Apply Boundary Conditions
// ---------------------------------------------------------------------------------------------------------------------
void ElectroMagnBC3D_SM::apply( ElectroMagn *EMfields, double time_dual, Patch *patch )
{

    // Static cast of the fields
    double *Ex3D = &(EMfields->Ex_->data_[0]);
    double *Ey3D = &(EMfields->Ey_->data_[0]);
    double *Ez3D = &(EMfields->Ez_->data_[0]);
    double *Bx3D = &(EMfields->Bx_->data_[0]);
    double *By3D = &(EMfields->By_->data_[0]);
    double *Bz3D = &(EMfields->Bz_->data_[0]);
    vector<double> pos( 2 );

    vector<double> byW( ny_p*nz_d, 0. );
    vector<double> byE( ny_p*nz_d, 0. );
    vector<double> bzW( ny_d*nz_p, 0. );
    vector<double> bzE( ny_d*nz_p, 0. );

    double* Bx_ext = NULL;
    if (Bx_val!=nullptr)
        Bx_ext = &(Bx_val->data_[0]);
    double* By_ext = NULL;
    if (By_val!=nullptr)
        By_ext = &(By_val->data_[0]);
    double* Bz_ext = NULL;
    if (Bz_val!=nullptr)
        Bz_ext = &(Bz_val->data_[0]);
    
    if( min_max==0 && patch->isXmin() ) {
    
        // for By^(d,p,d)
        for( unsigned int j=patch->isYmin() ; j<ny_p-patch->isYmax() ; j++ ) {
            pos[0] = patch->getDomainLocalMin( 1 ) + ( ( int )j - ( int )EMfields->oversize[1] )*dy;
            for( unsigned int k=patch->isZmin() ; k<nz_d-patch->isZmax() ; k++ ) {
                pos[1] = patch->getDomainLocalMin( 2 ) + ( ( int )k -0.5 - ( int )EMfields->oversize[2] )*dz;
                // Lasers
                for( unsigned int ilaser=0; ilaser< vecLaser.size(); ilaser++ ) {
                    byW[ j*nz_d+k ] += vecLaser[ilaser]->getAmplitude0( pos, time_dual, j, k );
                }
            }
        }
        for( unsigned int j=patch->isYmin() ; j<ny_p-patch->isYmax() ; j++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_d-patch->isZmax() ; k++ ) {
                
                By3D[ 0*(ny_p*nz_d) + j*nz_d + k ] = Alpha_SM_W   * Ez3D[ 0*(ny_p*nz_d) + j*nz_d + k ]
                                       +              Beta_SM_W    *( By3D[ 1*(ny_p*nz_d) + j*nz_d + k ]-By_ext[ j*nz_d + k ] )
                                       +              Gamma_SM_W   * byW[ j*nz_d+k ]
                                       +              Delta_SM_W   *( Bx3D[ 0*(ny_d*nz_d) + (j+1)*nz_d + k ]-Bx_ext[ (j+1)*nz_d + k ] )
                                       +              Epsilon_SM_W *( Bx3D[ 0*(ny_d*nz_d) +  j   *nz_d + k ]-Bx_ext[  j   *nz_d + k ] )
                                       + By_ext[ j*nz_d + k ];
            }// k  ---end compute By
        }//j  ---end compute By
        
        // for Bz^(d,d,p)
        for( unsigned int j=patch->isYmin() ; j<ny_d-patch->isYmax() ; j++ ) {
            pos[0] = patch->getDomainLocalMin( 1 ) + ( ( int )j - 0.5 - ( int )EMfields->oversize[1] )*dy;
            for( unsigned int k=patch->isZmin() ; k<nz_p-patch->isZmax() ; k++ ) {
                pos[1] = patch->getDomainLocalMin( 2 ) + ( ( int )k - ( int )EMfields->oversize[2] )*dz;
                // Lasers
                for( unsigned int ilaser=0; ilaser< vecLaser.size(); ilaser++ ) {
                    bzW[ j*nz_p+k ] += vecLaser[ilaser]->getAmplitude1( pos, time_dual, j, k );
                }
            }
        }
        for( unsigned int j=patch->isYmin() ; j<ny_d-patch->isYmax() ; j++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_p-patch->isZmax() ; k++ ) {
                Bz3D[ 0*(ny_d*nz_p) + j*nz_p + k ] = - Alpha_SM_W   * Ey3D[ 0*(ny_d*nz_p) + j*nz_p + k ]
                                       +              Beta_SM_W    *( Bz3D[ 1*(ny_d*nz_p) + j*nz_p + k ]-Bz_ext[ j*nz_p + k ] )
                                       +              Gamma_SM_W   * bzW[ j*nz_p+k ]
                                       +              Zeta_SM_W    *( Bx3D[ 0*(ny_d*nz_d) + j*nz_d + k+1 ]-Bx_ext[ j*nz_d + (k+1) ] )
                                       +              Eta_SM_W     *( Bx3D[ 0*(ny_d*nz_d) + j*nz_d + k   ]-Bx_ext[ j*nz_d +  k    ] )
                                       + Bz_ext[ j*nz_p + k ];
                                       
            }// k  ---end compute Bz
        }//j  ---end compute Bz
    } else if( min_max==1 && patch->isXmax() ) {
    
        // for By^(d,p,d)
        for( unsigned int j=patch->isYmin() ; j<ny_p-patch->isYmax() ; j++ ) {
            pos[0] = patch->getDomainLocalMin( 1 ) + ( ( int )j - ( int )EMfields->oversize[1] )*dy;
            for( unsigned int k=patch->isZmin() ; k<nz_d-patch->isZmax() ; k++ ) {
                pos[1] = patch->getDomainLocalMin( 2 ) + ( ( int )k - 0.5 - ( int )EMfields->oversize[2] )*dz;
                // Lasers
                for( unsigned int ilaser=0; ilaser< vecLaser.size(); ilaser++ ) {
                    byE[ j*nz_d+k ] += vecLaser[ilaser]->getAmplitude0( pos, time_dual, j, k );
                }
            }
        }
        for( unsigned int j=patch->isYmin() ; j<ny_p-patch->isYmax() ; j++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_d-patch->isZmax() ; k++ ) {
                By3D[ (nx_d-1)*(ny_p*nz_d) + j*nz_d + k ] = Alpha_SM_E   * Ez3D[ (nx_p-1)*(ny_p*nz_d) + j*nz_d + k ]
                                            +                   Beta_SM_E    *( By3D[ (nx_d-2)*(ny_p*nz_d) + j*nz_d + k ] -By_ext[ j*nz_d + k ] )
                                            +                   Gamma_SM_E   * byE[ j*nz_d+k ]
                                            +                   Delta_SM_E   *( Bx3D[ (nx_p-1)*(ny_d*nz_d) + (j+1)*nz_d + k ] -Bx_ext[ (j+1)*nz_d + k ] ) // Check x-index
                                            +                   Epsilon_SM_E *( Bx3D[ (nx_p-1)*(ny_d*nz_d) +  j   *nz_d + k ] -Bx_ext[  j   *nz_d + k ] )
                                            + By_ext[ j*nz_d + k ];
                                            
            }//k  ---end compute By
        }//j  ---end compute By
        
        // for Bz^(d,d,p)
        for( unsigned int j=patch->isYmin() ; j<ny_d-patch->isYmax(); j++ ) {
            pos[0] = patch->getDomainLocalMin( 1 ) + ( ( int )j - 0.5 - ( int )EMfields->oversize[1] )*dy;
            for( unsigned int k=patch->isZmin() ; k<nz_p-patch->isZmax() ; k++ ) {
                pos[1] = patch->getDomainLocalMin( 2 ) + ( ( int )k - ( int )EMfields->oversize[2] )*dz;
                // Lasers
                for( unsigned int ilaser=0; ilaser< vecLaser.size(); ilaser++ ) {
                    bzE[ j*nz_p+k ] += vecLaser[ilaser]->getAmplitude1( pos, time_dual, j, k );
                }
            }
        }
        for( unsigned int j=patch->isYmin() ; j<ny_d-patch->isYmax(); j++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_p-patch->isZmax() ; k++ ) {
                Bz3D[ (nx_d-1)*(ny_d*nz_p) + j*nz_p + k ] = -Alpha_SM_E * Ey3D[ (nx_p-1)*(ny_d*nz_p) + j*nz_p + k ]
                                            +                    Beta_SM_E  *( Bz3D[ (nx_d-2)*(ny_d*nz_p) + j*nz_p + k ] -Bz_ext[ j*nz_p + k ] )
                                            +                    Gamma_SM_E * bzE[ j*nz_p+k ]
                                            +                    Zeta_SM_E  *( Bx3D[ (nx_p-1)*(ny_d*nz_d) + j*nz_d + k+1 ]-Bx_ext[ j*nz_d + (k+1) ] )
                                            +                    Eta_SM_E   *( Bx3D[ (nx_p-1)*(ny_d*nz_d) + j*nz_d + k   ]-Bx_ext[ j*nz_d +  k    ] )
                                            + Bz_ext[ j*nz_p + k ];
            }//k  ---end compute Bz
        }//j  ---end compute Bz
    } else if( min_max==2 && patch->isYmin() ) {
    
        // for Bx^(p,d,d)
        for( unsigned int i=patch->isXmin() ; i<nx_p-patch->isXmax() ; i++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_d-patch->isZmax() ; k++ ) {
                Bx3D[ i*(ny_d*nz_d) + 0*nz_d + k ] = - Alpha_SM_S   * Ez3D[ i*(ny_p*nz_d) + 0*nz_d + k ]
                                       +              Beta_SM_S     *( Bx3D[  i   *(ny_d*nz_d) + 1*nz_d + k ]-Bx_ext[  i   *nz_d + k ] )
                                       +              Zeta_SM_S     *( By3D[ (i+1)*(ny_p*nz_d) + 0*nz_d + k ]-By_ext[ (i+1)*nz_d + k ] )
                                       +              Eta_SM_S      *( By3D[  i   *(ny_p*nz_d) + 0*nz_d + k ]-By_ext[  i   *nz_d + k ] )
                                       + Bx_ext[ i*nz_d + k ];
            }// k  ---end compute Bx
        }//i  ---end compute Bx
        
        // for Bz^(d,d,p)
        for( unsigned int i=patch->isXmin() ; i<nx_d-patch->isXmax() ; i++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_p-patch->isZmax() ; k++ ) {
                Bz3D[ i*(ny_d*nz_p) + 0*nz_p + k ] = Alpha_SM_S   * Ex3D[ i*(ny_p*nz_p) + 0*nz_p + k ]
                                       +              Beta_SM_S    *( Bz3D[ i*(ny_d*nz_p) + 1*nz_p + k   ]-Bz_ext[ i*nz_p +  k    ] )
                                       +              Delta_SM_S   *( By3D[ i*(ny_p*nz_d) + 0*nz_d + k+1 ]-By_ext[ i*nz_d + (k+1) ] )
                                       +              Epsilon_SM_S *( By3D[ i*(ny_p*nz_d) + 0*nz_d + k   ]-By_ext[ i*nz_d +  k    ] )
                                       + Bz_ext[ i*nz_p + k ];
            }// k  ---end compute Bz
        }//i  ---end compute Bz       }
    } else if( min_max==3 && patch->isYmax() ) {
    
        // for Bx^(p,d,d)
        for( unsigned int i=patch->isXmin() ; i<nx_p-patch->isXmax() ; i++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_d-patch->isZmax() ; k++ ) {
            
                Bx3D[ i*(ny_d*nz_d) + (ny_d-1)*nz_d + k ] = -Alpha_SM_N * Ez3D[ i*(ny_p*nz_d) + (ny_p-1)*nz_d + k ]
                                            +                    Beta_SM_N  *( Bx3D[  i   *(ny_d*nz_d) + (ny_d-2)*nz_d + k ]-Bx_ext[  i   *nz_d + k ] )
                                            +                    Zeta_SM_N  *( By3D[ (i+1)*(ny_p*nz_d) + (ny_p-1)*nz_d + k ]-By_ext[ (i+1)*nz_d + k ] )
                                            +                    Eta_SM_N   *( By3D[  i   *(ny_p*nz_d) + (ny_p-1)*nz_d + k ]-By_ext[  i   *nz_d + k ] )
                                            + Bx_ext[ i*nz_d + k ];
                                            
            }//k  ---end compute Bz
        }//j  ---end compute Bz
        
        // for Bz^(d,d,p)
        for( unsigned int i=patch->isXmin() ; i<nx_d-patch->isXmax() ; i++ ) {
            for( unsigned int k=patch->isZmin() ; k<nz_p-patch->isZmax() ; k++ ) {
            
                Bz3D[ i*(ny_d*nz_p) + (ny_d-1)*nz_p + k ] = Alpha_SM_N   * Ex3D[ i*(ny_p*nz_p) + (ny_p-1)*nz_p + k ]
                                            +                   Beta_SM_N    *( Bz3D[ i*(ny_d*nz_p) + (ny_d-2)*nz_p + k   ] -Bz_ext[ i*nz_p +  k    ] )
                                            +                   Delta_SM_N   *( By3D[ i*(ny_p*nz_d) + (ny_p-1)*nz_d + k+1 ] -By_ext[ i*nz_d + (k+1) ] )
                                            +                   Epsilon_SM_N *( By3D[ i*(ny_p*nz_d) + (ny_p-1)*nz_d + k   ] -By_ext[ i*nz_d +  k    ] )
                                            + Bz_ext[ i*nz_p + k ];
                                            
            }//k  ---end compute Bz
        }//j  ---end compute Bz
    } else if( min_max==4 && patch->isZmin() ) {
    
        // for Bx^(p,d,d)
        for( unsigned int i=patch->isXmin() ; i<nx_p-patch->isXmax() ; i++ ) {
            for( unsigned int j=patch->isYmin() ; j<ny_d-patch->isYmax() ; j++ ) {
            
                Bx3D[ i*(ny_d*nz_d) + j*(nz_d) + 0 ] = Alpha_SM_B   * Ey3D[ i*(ny_d*nz_p) + j*(nz_p) + 0 ]
                                       +              Beta_SM_B    *( Bx3D[  i   *(ny_d*nz_d) + j*(nz_d) + 1 ]-Bx_ext[  i   *ny_d + j ] )
                                       +              Delta_SM_B   *( Bz3D[ (i+1)*(ny_d*nz_p) + j*(nz_p) + 0 ]-Bz_ext[ (i+1)*ny_d + j ] )
                                       +              Epsilon_SM_B *( Bz3D[  i   *(ny_d*nz_p) + j*(nz_p) + 0 ]-Bz_ext[  i   *ny_d + j ] )
                                       + Bx_ext[ i*ny_d + j ];
            }// j  ---end compute Bx
        }//i  ---end compute Bx
        
        // for By^(d,p,d)
        for( unsigned int i=patch->isXmin() ; i<nx_d-patch->isXmax() ; i++ ) {
            for( unsigned int j=patch->isYmin() ; j<ny_p-patch->isYmax() ; j++ ) {
            
                By3D[ i*(ny_p*nz_d)+ j*(nz_d) + 0 ] = - Alpha_SM_B   * Ex3D[ i*(ny_p*nz_p)+ j*(nz_p) + 0 ]
                                       +              Beta_SM_B   *( By3D[ i*(ny_p*nz_d)+  j   *(nz_d) + 1 ]-By_ext[ i*ny_p +  j   ] )
                                       +              Zeta_SM_B   *( Bz3D[ i*(ny_d*nz_p)+ (j+1)*(nz_p) + 0 ]-Bz_ext[ i*ny_d + (j+1) ] )
                                       +              Eta_SM_B    *( Bz3D[ i*(ny_d*nz_p)+  j   *(nz_p) + 0 ]-Bz_ext[ i*ny_d +  j   ] )
                                       + By_ext[ i*ny_p + j ];
                                       
            }// j  ---end compute By
        }//i  ---end compute By
        
    } else if( min_max==5 && patch->isZmax() ) {
    
        // for Bx^(p,d,d)
        for( unsigned int i=patch->isXmin() ; i<nx_p-patch->isXmax() ; i++ ) {
            for( unsigned int j=patch->isYmin() ; j<ny_d-patch->isYmax() ; j++ ) {
            
                Bx3D[ i*(ny_d*nz_d) + j*(nz_d) + (nz_d-1) ] = Alpha_SM_T   * Ey3D[ i*(ny_d*nz_p) + j*(nz_p) + (nz_p-1) ]
                                            +                   Beta_SM_T    *( Bx3D[  i   *(ny_d*nz_d) + j*(nz_d) + (nz_d-2) ] -Bx_ext[  i   *ny_d + j ] )
                                            +                   Delta_SM_T   *( Bz3D[ (i+1)*(ny_d*nz_p) + j*(nz_p) + (nz_p-1) ] -Bz_ext[ (i+1)*ny_d + j ] )
                                            +                   Epsilon_SM_T *( Bz3D[  i   *(ny_d*nz_p) + j*(nz_p) + (nz_p-1) ] -Bz_ext[  i   *ny_d + j ] )
                                            + Bx_ext[ i*ny_d + j ];
                                            
            }//j  ---end compute Bx
        }//i  ---end compute Bx
        
        
        // for By^(d,p,d)
        for( unsigned int i=patch->isXmin() ; i<nx_d-patch->isXmax() ; i++ ) {
            for( unsigned int j=patch->isYmin() ; j<ny_p-patch->isYmax() ; j++ ) {
            
                By3D[ i*(ny_p*nz_d) + j*(nz_d) + (nz_d-1) ] = -Alpha_SM_T * Ex3D[ i*(ny_p*nz_p) + j*(nz_p) + (nz_p-1) ]
                                            +                    Beta_SM_T  *( By3D[ i*(ny_p*nz_d) +  j   *(nz_d) + (nz_d-2) ]-By_ext[ i*ny_p +  j ] )
                                            +                    Zeta_SM_T  *( Bz3D[ i*(ny_d*nz_p) + (j+1)*(nz_p) + (nz_p-1) ]-Bz_ext[ i*ny_d + (j+1) ] )
                                            +                    Eta_SM_T   *( Bz3D[ i*(ny_d*nz_p) +  j   *(nz_p) + (nz_p-1) ]-Bz_ext[ i*ny_d +  j ] )
                                            + By_ext[ i*ny_p + j ];
                                            
            }//j  ---end compute By
        }//i  ---end compute By
        
    }
}
