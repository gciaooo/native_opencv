#include <iostream>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

extern "C" {
__attribute__((visibility("default"))) __attribute__((used))
const char *version() {
    return CV_VERSION;
}

__attribute__((visibility("default"))) __attribute__((used))
bool matchesSort(DMatch a, DMatch b) {
    return a.distance < b.distance;
}

__attribute__((visibility("default"))) __attribute__((used))
const void cropDocument(char *input_path, char *query_path, char *output_path) {
    //leggo le immagini che si trovano nei path presi da input
    Mat img = imread(input_path, IMREAD_GRAYSCALE);
    Mat imgQ = imread(query_path, IMREAD_GRAYSCALE);

    //creo il processo per l'algoritmo di ricerca dei keypoint
    Ptr<FeatureDetector> orb = ORB::create(1000);

    vector<KeyPoint> kp1, kp2;
    Mat des1, des2;

    //trovo i keypoint e le descrizioni dei keypoint per l'immagine di input e l'immagine del template
    orb->detectAndCompute(imgQ, noArray(), kp1, des1);
    orb->detectAndCompute(img, noArray(), kp2, des2);

    vector<DMatch> matches;
    //creo il processo per eseguire il Brute-Force matching
    BFMatcher bf = BFMatcher(NORM_HAMMING);

    //eseguo il Brute-Force Matching e ordino il risultato per le coppie di keypoint con la distanza minore
    bf.match(des2, des1, matches);
    sort(matches.begin(), matches.end(), matchesSort);

    //prendo un campione dei matches
    vector<DMatch> good = matches;
    good.resize(80);

    //    Mat imgMatch;
    //    drawMatches(img, kp2, imgQ, kp1, good, imgMatch, Scalar::all(-1), Scalar::all(-1),
    //                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //prendo le coordinate delle coppie di keypoints associati all'array good
    vector<Point2f> srcPoints, dstPoints;
    for (DMatch m: good) {
        srcPoints.push_back(kp2[m.queryIdx].pt);
        dstPoints.push_back(kp1[m.trainIdx].pt);
    }

    //trovo la trasformazione planare usando i keypoints
    Mat M = findHomography(srcPoints, dstPoints, RANSAC);

    //trasformo l'immagine di input e la salvo nel path dell'immagine di output
    Mat imgScan;
    warpPerspective(img, imgScan, M, imgQ.size());
    //       imwrite(output_path, imgMatch);
    imwrite(output_path, imgScan);
}

//__attribute__((visibility("default"))) __attribute__((used))
//const int detectAnswer(int** answers, int size, char* output_path) {
//    int count = 0;
//    for (int i = 0; i < size; i++) {
//        int x = answers[i][0], y = answers[i][1];
//        Mat img = imread(output_path, IMREAD_GRAYSCALE);
//        Vec3i color = img.at<Vec3i>(Point(x, y));
//        return color[0];
//    }
//}

}