#include <stdlib.h>
#include <string.h>
#include <stdio.h>                          /* Sobel.c */
#include <math.h>

        int pic[256][256];
        int outpicx[256][256];
        int outpicy[256][256];

        int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
        int maskxPadded[7][7] = {{0,0,0,0,0,0,0},
                                 {0,0,0,0,0,0,0},
                                 {0,0,-1,0,1,0,0},
                                 {0,0,-2,0,2,0,0},
                                 {0,0,-1,0,1,0,0},
                                 {0,0,0,0,0,0,0},
                                 {0,0,0,0,0,0,0},};
        int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
        int maskyPadded[7][7] = {{0,0,0,0,0,0,0},
                                 {0,0,0,0,0,0,0},
                                 {0,0,1,2,1,0,0},
                                 {0,0,0,0,0,0,0},
                                 {0,0,-1,-2,-1,0,0},
                                 {0,0,0,0,0,0,0},
                                 {0,0,0,0,0,0,0},};
        double ival[256][256],maxival;

//mpcen/RobotVision TODO: remove comments
// gcc newSobel.c -o mySobel.out
// ./mySobel.out face05.pgm output.pgm 5 7
//http://www-scf.usc.edu/~boqinggo/Canny.htm
// marrh.2  from 82 down ignore

void populateZeroArray(int paddedZeros[7][7], int pic[256][256], int i, int j);
int getSumX(int paddedZeros[7][7], int pic[256][256], int i, int j);
int getSumY(int paddedZeros[7][7], int pic[256][256], int i, int j); 

main(argc,argv)
int argc;
char **argv;
{
  int i,j,p,q,mr,sum1,sum2, pixel_i, pixel_j;
  double threshold, threshold2;
  FILE *fpOut, *fpLowThr, *fpHighThr, *fpImmage, *fopen();
  char *foobar;

  //Zero out all arrays
  int x = 256;
  int y = 256;
  memset(pic, 0, sizeof(pic[0][0]) * x * y);
  memset(outpicx, 0, sizeof(outpicx[0][0]) * x * y);
  memset(outpicy, 0, sizeof(outpicy[0][0]) * x * y);

  // First Argument: Input File (face05.pgm)
  argc--; argv++;
  foobar = *argv;
  fpImmage=fopen(foobar,"rb");

  // Second Argument: Output File (output.pgm)
	argc--; argv++;
	foobar = *argv;
	fpOut=fopen(foobar,"wb");

  // Third Argument: Threshold value
  // This value will act as the center threshold value.
  // The low and high threshold values will be relative to this value,
  // where low will be -50% and high will be +50%
  argc--; argv++;
	foobar = *argv;
	threshold = atof(foobar);

  argc--; argv++;
	foobar = *argv;
	threshold2 = atof(foobar);


  // Creates the two (low & high) threshold output images
  fpLowThr=fopen("lowthr.pgm", "wb");
  fpHighThr=fopen("highthr.pgm", "wb");

  ///////////////////////
  // Get and store image
  /////////////////////
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
      pic[i][j]  =  getc (fpImmage);
      pic[i][j]  &= 0x00FF;
    }
  }

  int paddedZeros[7][7];
  memset(paddedZeros, 0, sizeof(pic[0][0]) * 7 * 7);
  //////////////////////////////////////////////////
  //Scanning Convolution lec1-conv slide 11
  /////////////////////////////////////////////////
  mr = 1;
  for ( pixel_i = mr; pixel_i <256-mr; pixel_i++)
  { for (pixel_j=mr;pixel_j<256-mr;pixel_j++)
    {
        sum2 = 0; sum1 = 0;
        //////////////////////////////////////////////////
        //Calculate the wheighted Sum lec-2 Sobel slide 1
        /////////////////////////////////////////////////

        sum1 = getSumX( paddedZeros, pic, pixel_i, pixel_j);
        sum2 = getSumY( paddedZeros, pic, pixel_i, pixel_j);      
        //Store wheighted sum
        outpicx[pixel_i][pixel_j] = sum1;
        outpicy[pixel_i][pixel_j] = sum2;
    }
  }
  mr = 1;

        maxival = 0;
        for (i=mr;i<256-mr;i++)
        { for (j=mr;j<256-mr;j++)
          {
             //The magnitude of a vector is the square root of the numbers from the convolution
             ival[i][j]=sqrt((double)((outpicx[i][j]*outpicx[i][j]) +
                                      (outpicy[i][j]*outpicy[i][j])));
             //Store the maximum value
             if (ival[i][j] > maxival){
                maxival = ival[i][j];

            }

           }
        }

        //Added File header to magnitude output file
        fprintf(fpOut, "P5\n256 256\n255\n");

        for (i=0;i<256;i++)
          { for (j=0;j<256;j++)
            {
             ival[i][j] = (ival[i][j] / maxival) * 255;
             fprintf(fpOut,"%c",(char)((int)(ival[i][j])));

            }
          }

        // File headers for the low and high thresholded images
        fprintf(fpLowThr, "P5\n256 256\n255\n");
        fprintf(fpHighThr, "P5\n256 256\n255\n");

        // This is what specifies the low & high thresholds.
        // low = 50% lower than middle threshold which was entered
          // as an argument
        // high = 50% higher than the entered threshold

        for(i=0; i<256; i++){
          for(j=0; j<256; j++){
            //Low Threshold
            if(ival[i][j] > threshold)
              fprintf(fpLowThr, "%c", 255);
            else
              fprintf(fpLowThr, "%c", 0);
            //High Threshold
            if(ival[i][j] > threshold2)
              fprintf(fpHighThr, "%c", 255);
            else
              fprintf(fpHighThr, "%c", 0);
          }
        }


}

void populateZeroArray(int paddedZeros[7][7], int pic[256][256], int i, int j)
{

  memset(paddedZeros, 0, sizeof(pic[0][0]) * 7 * 7);
  
  int mr = 1;
  for(int x = -mr; x <= mr; x++){
    for(int y = -mr; y <= mr; y++){
      //2->4              //mr =  -1 -> 1
      paddedZeros[x+3][y+3] = pic[i+x][j+y];
    }
  }
}

int getSumX(int paddedZeros[7][7], int pic[256][256], int i, int j)
{

  memset(paddedZeros, 0, sizeof(pic[0][0]) * 7 * 7);

  int mr = 1;
  int sum = 0;
  for(int x = -mr; x <= mr; x++){
    for(int y = -mr; y <= mr; y++){
      //2->4              //mr =  -1 -> 1
      paddedZeros[x+3][y+3] = pic[i+x][j+y];
    }
  }

  for( int i = 0; i < 6; i ++ ){
    for( int j = 0; j < 6; j++ ){
      sum += paddedZeros[i][j] * maskxPadded[i][j];
    }
  }

  return sum;
}

int getSumY(int paddedZeros[7][7], int pic[256][256], int i, int j)
{
  memset(paddedZeros, 0, sizeof(pic[0][0]) * 7 * 7);

  int mr = 1;
  int sum = 0;
  for(int x = -mr; x <= mr; x++){
    for(int y = -mr; y <= mr; y++){
      //2->4              //mr =  -1 -> 1
      paddedZeros[x+3][y+3] = pic[i+x][j+y];
    }
  }

  for( int i = 0; i < 6; i ++ ){
    for( int j = 0; j < 6; j++ ){
      sum += paddedZeros[i][j] * maskyPadded[i][j];
    }
  }

  return sum;
}
