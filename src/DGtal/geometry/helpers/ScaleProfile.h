/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#pragma once

/**
 * @file ScaleProfile.h
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 * @Jacques-Olivier Lachaud
 *
 * @date 2015/11/08
 *
 * Header file for module ScaleProfile.cpp
 *
 * This file is part of the DGtal library.
 */

#if defined(ScaleProfile_RECURSES)
#error Recursive header files inclusion detected in ScaleProfile.h
#else // defined(ScaleProfile_RECURSES)
/** Prevents recursive inclusion of headers. */
#define ScaleProfile_RECURSES

#if !defined ScaleProfile_h
/** Prevents repeated inclusion of headers. */
#define ScaleProfile_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include <vector>
#include "DGtal/math/Statistic.h"

#include "DGtal/base/Common.h"
//////////////////////////////////////////////////////////////////////////////

namespace DGtal
{

  /////////////////////////////////////////////////////////////////////////////
  // class ScaleProfile
  /**
   * Description of class 'ScaleProfile' <p> \brief Aim: This class
   * represents one (multi)scale profile, i.e. a sequence of statistics
   * on digital lengths parameterized by a scale. This class only
   * represents what happens at one place, not everywhere on the
   * contour. All further computations in the scale profile are
   * done in logspace.
   * 
   * The proposed implementation is mainly a backport from
   * [ImaGene](https://gforge.liris.cnrs.fr/projects/imagene) with some
   * various refactoring.
   */


  class ScaleProfile
  {
    // ----------------------- Standard services ------------------------------
  public:

    /**
     * Used to specify the method to compute the profile values
     * (used in @ref getProfile())
     * 
     **/
    enum ProfileComputingType{MEAN, MAX, MIN, MEDIAN};


    /**
     * Destructor.
     */
    ~ScaleProfile();

    /**
     * Constructor. The object is not valid.
     */
    ScaleProfile();


    /**
     * Constructor. The object is not valid.
     * @param type allows to specify the used to computes the profile points from the added samples.
     */
    ScaleProfile(ProfileComputingType type);
    

    /**
     * Copy constructor.
     * @param other the object to clone.
     */
    ScaleProfile( const ScaleProfile & other );


    /**
     * Assignment.
     * @param other the object to copy.
     * @return a reference on 'this'.
     * Forbidden by default.
     */
    ScaleProfile & operator= ( const ScaleProfile & other );


 
    /**
     * Clears the object as if it has been just created.
     */
    void clear();



    /**
     * Initializer. Must be called before adding datas. Specifies the
     * scales of the profile (generally an iterator on a sequence
     * (1,2,3,4,...N)).
     *
     * @param an iterator pointing on the first scale (some
     * floating-point convertible value).
     *
     * @param an iterator pointing after the last scale.
     */
    template <typename Iterator>
    void init( Iterator begin_scale, Iterator end_scale, bool storeValsInStats=false );



    /**
     * Initializer. Must be called before adding datas. Specifies the
     * scales of the profile as the sequence (1,2,3,4,...,nb).
     *
     * @param nb an integer number strictly positive.
     */
    void init( uint nb, bool storeValsInStats=false );


    /**
     * Adds some sample value at the given scale.
     *
     * @param idx_scale some valid index (according to init).
     * @param value any value.
     */
    void addValue( uint idx_scale, float value );


    /**
     * Adds some statistic at the given scale.
     *
     * @param idx_scale some valid index (according to init).
     *
     * @param stat any statistic (which is added to the current
     * statistic object).
     */
    void addStatistic( uint idx_scale, const Statistic<float> & stat );



    /**
     * It stops and Erase the stats saved values. It must be called to
     * avoid to store all statistics values when we have to acess to
     * the median value.  Typically if you nedd to access to the
     * median value of the profile, you need to follow this example:
     * @code
     * ScaleProfile sp;
     * // the value are now stored and you can access to the median value of the profile.
     * sp.init(true);
     * sp.addValue(0, 10.5);
     * sp.addValue(0, 9.2);
     *  ...
     * // When all values have been added you can stop to store them again
     * sp.stopStatsSaving();
     * // before to erase the statistics sample values the median is computed and stored.
     * @endcode
     **/
    void stopStatsSaving() ;    




    // ----------------------- Profile services --------------------------------
  public:
    
    /**
     * Used to define the method to determine the profile values from
     * the samples of scale statistics.
     *
     * @param type the method applied to the statistics samples: MEAN, MAX, MIN.
     **/
    
    
    void setProfileDef(ProfileComputingType type);
    

    
    /**
     * Used by boost to Serialize. 
     *
     **/
   
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
      ar & m_scales & m_stats;
    }

    /**
     * @param x (modified) adds the x-value of the profile (log(scale))
     * to the back of the vector.
     *
     * @param y (modified) adds the y-value of the profile
     * (log(Exp(samples))) to the back of the vector.
     */
    void getProfile( std::vector<double> & x, 
		     std::vector<double> & y ) const;
    

    /**
     * A meaningful scale is an interval of scales of length no
     * smaller than [min_width] and in which the profile has slopes
     * below [max_slope] and above [min_slope]. This method returns
     * the sequence of meaningful scales for surfel [idx].
     *
     * @param intervals (returns) a list of meaningful scales.
     * @param min_width the minimum length for the meaningful scales.
     * @param max_slope the maximum allowed slope for length evolution.
     * @param min_slope the minimum allowed slope for length evolution.
     */
    void 
    meaningfulScales( std::vector< std::pair< uint, uint > > & intervals,
		      uint min_width = 1,
		      double max_slope = -0.2,
		      double min_slope = -1e10 ) const;

    /**
     *  Compute the profile slope of the first meaningful scale
     *  interval computed by a simple linear regression model.
     *
     * @return a pair<bool, double> giving the slope and indicating if
     * the a meaningful scale was well found or not. If no meaningful
     * scale interval was found, it simply return the slope obtained
     * from the linear regression. 
     *
     * @param min_slope the  minimum allowed slope for length evolution.  
     * @param max_slope the  maximum allowed slope for length evolution.
     * 
     **/

    
    std::pair<bool, double> 
    getSlopeFromMeaningfulScales(double max_slope=-0.2, double min_slope=-1e10, uint min_size=2) const ;
    




   
    
    /**
     * The noise level is the first scale of the first meaningful
     * scale. A meaningful scale is an interval of scales of length no
     * smaller than [min_width] and in which the profile has slopes
     * below [max_slope]. 
     *
     * @param min_width the minimum length for the meaningful scales.
     * @param max_slope the maximum allowed slope for length evolution.
     * @param min_slope the minimum allowed slope for length evolution.
     * @return the noise level or zero is none was found.
     * @see meaningfulScales
     */
    uint
    noiseLevel( uint min_width = 1,
		double max_slope = -0.2,
		double min_slope = -1e10 ) const;


    /**
     * The noise level is the first scale of the first meaningful
     * scale. A meaningful scale is an interval of scales of length no
     * smaller than [min_width] and in which the profile has slopes
     * below [max_slope]. The lower bounded noise level also requires
     * minimum lenghs for different scales. Therefore the profile must
     * be greater that
     * [lower_bound_at_scale_1]+[lower_bound_slope]*scale.
     *
     * @param min_width the minimum length for the meaningful scales.
     * @param max_slope the maximum allowed slope for length evolution.
     * @param min_slope the minimum allowed slope for length evolution.
     * @param lower_bound_at_scale_1 the lower bound for the profile at scale 1.
     * @param lower_bound_slope the slope of the lower bound for the profile (for instance -1 for digital contours, -3 for digital image graphs since area values are divided by (scale)^3.
     * @return the noise level or zero is none was found.
     * @see meaningfulScales
     */
    uint
    lowerBoundedNoiseLevel( uint min_width = 1,
			    double max_slope = -0.2,
			    double min_slope = -1e10,
			    double lower_bound_at_scale_1 = 1.0,
			    double lower_bound_slope = -2.0 ) const;




    

    // ----------------------- Interface --------------------------------------
  public:

    /**
     * Writes/Displays the object on an output stream.
     * @param out the output stream where the object is written.
     */
    void selfDisplay ( std::ostream & out ) const;

    /**
     * Checks the validity/consistency of the object.
     * @return 'true' if the object is valid, 'false' otherwise.
     */
    bool isValid() const;

    // ------------------------- Protected Datas ------------------------------
  private:
    // ------------------------- Private Datas --------------------------------
  private:

    /**
     * The vector containing the different scales for the analysis.
     */
    std::vector<float>* m_scales;

    /**
     * The vector containing the different statistics for the analysis.
     */
    std::vector< Statistic<float> >* m_stats;

    /**
     * Used to define the method to compute the scale profile: several choice are possible:
     * MEAN (default), MAX, MIN (not efficient)
     */
    
    ProfileComputingType m_profileDef;
    


    /**
     * Used to temporaly store values in statistics in order to be
     * able to access to the median value. By default the value is set
     * to false and the median is not available. 
     * @see setStoreStats
     */
    bool m_storeValInStats;
    


    // ------------------------- Hidden services ------------------------------
  protected:


  private:

  

 
    // ------------------------- Internals ------------------------------------
  private:

  }; // end of class ScaleProfile


  /**
   * Overloads 'operator<<' for displaying objects of class 'ScaleProfile'.
   * @param out the output stream where the object is written.
   * @param object the object of class 'ScaleProfile' to write.
   * @return the output stream after the writing.
   */
  std::ostream&
  operator<< ( std::ostream & out, const ScaleProfile & object );


} // namespace DGtal


///////////////////////////////////////////////////////////////////////////////
// Includes inline functions.
#if !defined(BUILD_INLINE)
#include "DGtal/geometry/helpers/ScaleProfile.ih"
#endif


//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#endif // !defined ScaleProfile_h

#undef ScaleProfile_RECURSES
#endif // else defined(ScaleProfile_RECURSES)
