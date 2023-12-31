/**
 * @file qtree.cpp
 * @description student implementation of QTree class used for storing image data
 *              CPSC 221 PA3
 *
 *              SUBMIT THIS FILE
 */

#include "qtree.h"

/**
 * Constructor that builds a QTree out of the given PNG.
 * Every leaf in the tree corresponds to a pixel in the PNG.
 * Every non-leaf node corresponds to a rectangle of pixels
 * in the original PNG, represented by an (x,y) pair for the
 * upper left corner of the rectangle and an (x,y) pair for
 * lower right corner of the rectangle. In addition, the Node
 * stores a pixel representing the average color over the
 * rectangle.
 *
 * The average color for each node in your implementation MUST
 * be determined in constant time. HINT: this will lead to nodes
 * at shallower levels of the tree to accumulate some error in their
 * average color value, but we will accept this consequence in
 * exchange for faster tree construction.
 * Note that we will be looking for specific color values in our
 * autograder, so if you instead perform a slow but accurate
 * average color computation, you will likely fail the test cases!
 *
 * Every node's children correspond to a partition of the
 * node's rectangle into (up to) four smaller rectangles. The node's
 * rectangle is split evenly (or as close to evenly as possible)
 * along both horizontal and vertical axes. If an even split along
 * the vertical axis is not possible, the extra line will be included
 * in the left side; If an even split along the horizontal axis is not
 * possible, the extra line will be included in the upper side.
 * If a single-pixel-wide rectangle needs to be split, the NE and SE children
 * will be null; likewise if a single-pixel-tall rectangle needs to be split,
 * the SW and SE children will be null.
 *
 * In this way, each of the children's rectangles together will have coordinates
 * that when combined, completely cover the original rectangle's image
 * region and do not overlap.
 */
QTree::QTree(const PNG& imIn) {
	width = imIn.width();
    height = imIn.height();

	root = BuildNode(imIn, make_pair(0, 0), make_pair(width - 1, height - 1));
}


/**
 * Overloaded assignment operator for QTrees.
 * Part of the Big Three that we must define because the class
 * allocates dynamic memory. This depends on your implementation
 * of the copy and clear funtions.
 *
 * @param rhs The right hand side of the assignment statement.
 */
QTree& QTree::operator=(const QTree& rhs) {
	// check for self assignment 
    if (this == &rhs) {
        return *this;
    }

    // clear the current tree
    Clear(); 

    // copy the data from rhs
    Copy(rhs); 

    return *this;
	
}

/**
 * Render returns a PNG image consisting of the pixels
 * stored in the tree. may be used on pruned trees. Draws
 * every leaf node's rectangle onto a PNG canvas using the
 * average color stored in the node.
 *
 * For up-scaled images, no color interpolation will be done;
 * each rectangle is fully rendered into a larger rectangular region.
 *
 * @param scale multiplier for each horizontal/vertical dimension
 * @pre scale > 0
 */
PNG QTree::Render(unsigned int scale) const {
    if (!root) {
        return PNG();  // empty image
    }

    //dimensions of the scaled image
    unsigned int newWidth = (root->lowRight.first - root->upLeft.first + 1) * scale;
    unsigned int newHeight = (root->lowRight.second - root->upLeft.second + 1) * scale;
    PNG img(newWidth, newHeight);

    // render each leaf node
    renderNode(img, root, scale);

    return img;
}

/**
 *  Prune function trims subtrees as high as possible in the tree.
 *  A subtree is pruned (cleared) if all of the subtree's leaves are within
 *  tolerance of the average color stored in the root of the subtree.
 *  NOTE - you may use the distanceTo function found in RGBAPixel.h
 *  Pruning criteria should be evaluated on the original tree, not
 *  on any pruned subtree. (we only expect that trees would be pruned once.)
 *
 * You may want a recursive helper function for this one.
 *
 * @param tolerance maximum RGBA distance to qualify for pruning
 * @pre this tree has not previously been pruned, nor is copied from a previously pruned tree.
 */
void QTree::Prune(double tolerance) {
    pruneHelper(root, tolerance);
}


/**
 *  FlipHorizontal rearranges the contents of the tree, so that
 *  its rendered image will appear mirrored across a vertical axis.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  After flipping, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel wide rectangles have
 *  null eastern children
 *  (i.e. after flipping, a node's NW and SW pointers may be null, but
 *  have non-null NE and SE)
 * 
 *  You may want a recursive helper function for this one.
 */
void QTree::FlipHorizontal() {

    if (root) {
        flipHorizontalHelper(root, root->lowRight.first); 
    }

}

void QTree::flipHorizontalHelper(Node* node, unsigned int imageWidth) {
    if (!node) return;

	 unsigned int newLeftX = 0;
	 unsigned int newRightX = 0;

	newLeftX = imageWidth - node->lowRight.first;
    newRightX = imageWidth - node->upLeft.first;

 

    node->upLeft.first = newLeftX;
    node->lowRight.first = newRightX;

    if (node->NW) flipHorizontalHelper(node->NW, imageWidth);
    if (node->NE) flipHorizontalHelper(node->NE, imageWidth);
    if (node->SW) flipHorizontalHelper(node->SW, imageWidth);
    if (node->SE) flipHorizontalHelper(node->SE, imageWidth);

    std::swap(node->NW, node->NE);
    std::swap(node->SW, node->SE);
}


/**
 *  RotateCCW rearranges the contents of the tree, so that its
 *  rendered image will appear rotated by 90 degrees counter-clockwise.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  Note that this may alter the dimensions of the rendered image, relative
 *  to its original dimensions.
 *
 *  After rotation, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel tall or wide rectangles
 *  have null eastern or southern children
 *  (i.e. after rotation, a node's NW and NE pointers may be null, but have
 *  non-null SW and SE, or it may have null NW/SW but non-null NE/SE)
 *
 *  You may want a recursive helper function for this one.
 */
void QTree::RotateCCW() {
    if (root) {
        rotateCCWHelper(root, width, height);
		
    }
	swap(width, height); // swap width and height
    

}

void QTree::rotateCCWHelper(Node* node, unsigned int imageWidth, unsigned int imageHeight) {
    if (!node) {
        return; // base
    }


    // rotate node coordinates
    unsigned int newUpLeftX = 0;
    unsigned int newUpLeftY = 0;
    unsigned int newLowRightX = 0;
    unsigned int newLowRightY = 0; 

	newUpLeftX = node->upLeft.second;
	newUpLeftY =imageWidth - node->lowRight.first - 1;
	newLowRightX = node->lowRight.second;
	newLowRightY = imageWidth - node->upLeft.first - 1;


    node->upLeft = make_pair(newUpLeftX, newUpLeftY);
    node->lowRight = make_pair(newLowRightX, newLowRightY);


 	// reassign for CCW
    Node* tempNW = node->NW;
    Node* tempNE = node->NE;
    Node* tempSW = node->SW;
    Node* tempSE = node->SE;

   
    node->NW = tempNE; 
    node->NE = tempSE; 
    node->SE = tempSW; 
    node->SW = tempNW; 


    // rotate the child nodes
    rotateCCWHelper(node->NW, imageWidth, imageHeight); 
    rotateCCWHelper(node->NE, imageWidth, imageHeight);
    rotateCCWHelper(node->SW, imageWidth, imageHeight); 
    rotateCCWHelper(node->SE, imageWidth, imageHeight); 
}





/**
 * Destroys all dynamically allocated memory associated with the
 * current QTree object. Complete for PA3.
 * You may want a recursive helper function for this one.
 */
void QTree::Clear() {
    clearHelper(root);
    root = nullptr; 
}

/**
 * Copies the parameter other QTree into the current QTree.
 * Does not free any memory. Called by copy constructor and operator=.
 * You may want a recursive helper function for this one.
 * @param other The QTree to be copied.
 */
void QTree::Copy(const QTree& other) {
    root = copyHelper(other.root);
}

/**
 * Private helper function for the constructor. Recursively builds
 * the tree according to the specification of the constructor.
 * @param img reference to the original input image.
 * @param ul upper left point of current node's rectangle.
 * @param lr lower right point of current node's rectangle.
 */


Node* QTree::BuildNode(const PNG & img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr) {
    if (ul.first >= img.width() || ul.second >= img.height() || lr.first < ul.first || lr.second < ul.second) {
        return nullptr;  
    }


    if (ul == lr) {
        RGBAPixel* pixel = img.getPixel(ul.first, ul.second);
        if (!pixel) {
            return nullptr;
        }
        return new Node(ul, lr, *pixel);
    }

    Node* node = new Node(ul, lr, RGBAPixel());
    if (!node) {
        return nullptr;
    }

	width = lr.first - ul.first + 1;
	height = lr.second - ul.second + 1;


    unsigned int midX = ul.first + width / 2;
    unsigned int midY = ul.second + height / 2;

    if (width % 2 == 0) midX--;
    if (height % 2 == 0) midY--;

    if (width == 1) {
        node->NW = BuildNode(img, ul, make_pair(ul.first, midY));
        node->SW = (height > 1) ? BuildNode(img, make_pair(ul.first, midY + 1), lr) : nullptr;
        node->NE = nullptr;
        node->SE = nullptr;
    } else if (height == 1) {
        node->NW = BuildNode(img, ul, make_pair(midX, ul.second));
        node->NE = BuildNode(img, make_pair(midX + 1, ul.second), lr);
        node->SW = nullptr;
        node->SE = nullptr;
    } else {
        node->NW = BuildNode(img, ul, make_pair(midX, midY));
        node->NE = BuildNode(img, make_pair(midX + 1, ul.second), make_pair(lr.first, midY));
        node->SW = BuildNode(img, make_pair(ul.first, midY + 1), make_pair(midX, lr.second));
        node->SE = BuildNode(img, make_pair(midX + 1, midY + 1), lr);
    }

    if (node->NW || node->NE || node->SW || node->SE) {
        node->avg = calculateAverageColour(node);
    }

    return node;
}





/*********************************************************/
/*** IMPLEMENT YOUR OWN PRIVATE MEMBER FUNCTIONS BELOW ***/
/*********************************************************/

RGBAPixel QTree::calculateAverageColour(Node* node) {
    if (!node || isLeaf(node)) {
        return node ? node->avg : RGBAPixel();
    }

    unsigned long long totalR = 0, totalG = 0, totalB = 0;
    unsigned int totalArea = 0;

    auto accumulateChildColor = [&](Node* child) {
        if (child) {
            unsigned int childArea = (child->lowRight.first - child->upLeft.first + 1) * 
                                     (child->lowRight.second - child->upLeft.second + 1);
            totalR += static_cast<unsigned long long>(child->avg.r) * childArea;
            totalG += static_cast<unsigned long long>(child->avg.g) * childArea;
            totalB += static_cast<unsigned long long>(child->avg.b) * childArea;
            totalArea += childArea;
        }
    };

    accumulateChildColor(node->NW);
    accumulateChildColor(node->NE);
    accumulateChildColor(node->SW);
    accumulateChildColor(node->SE);

    unsigned char avgR = static_cast<unsigned char>(totalArea ? totalR / totalArea : 0);
    unsigned char avgG = static_cast<unsigned char>(totalArea ? totalG / totalArea : 0);
    unsigned char avgB = static_cast<unsigned char>(totalArea ? totalB / totalArea : 0);

    return RGBAPixel(avgR, avgG, avgB);
}


void QTree::renderNode(PNG & img, Node* node, unsigned int scale) const {
    if (!node) {
        return;
    }

    // check if the node is a leaf node
    if (!node->NW && !node->NE && !node->SW && !node->SE) {
        // rectangle bounds of the node
        unsigned int startX = node->upLeft.first * scale;
        unsigned int startY = node->upLeft.second * scale;
        unsigned int endX = (node->lowRight.first + 1) * scale;
        unsigned int endY = (node->lowRight.second + 1) * scale;

        // draw rectangle
        for (unsigned int x = startX; x < endX; x++) {
            for (unsigned int y = startY; y < endY; y++) {
                if (x < img.width() && y < img.height()) {
                    RGBAPixel *pixel = img.getPixel(x, y);
                    *pixel = node->avg;
            	}
			}
        }
    } else {
        //  render child nodes
        if (node->NW) renderNode(img, node->NW, scale);
        if (node->NE) renderNode(img, node->NE, scale);
        if (node->SW) renderNode(img, node->SW, scale);
        if (node->SE) renderNode(img, node->SE, scale);
    }
}





void QTree::clearHelper(Node* node) {
    if (!node) {
        return; 
    }

    // recursively clear child nodes
    clearHelper(node->NW);
    clearHelper(node->NE);
    clearHelper(node->SW);
    clearHelper(node->SE);

    delete node;
}

Node* QTree::copyHelper(Node* otherNode) {
    if (!otherNode) {
        return nullptr; 
    }

    // new node with the same data as otherNode
    Node* newNode = new Node(otherNode->upLeft, otherNode->lowRight, otherNode->avg);

    // copy child nodes
    newNode->NW = copyHelper(otherNode->NW);
    newNode->NE = copyHelper(otherNode->NE);
    newNode->SW = copyHelper(otherNode->SW);
    newNode->SE = copyHelper(otherNode->SE);

    return newNode;
}


void QTree::pruneHelper(Node* node, double tolerance) {
    if (!node) return; 

    if (isLeaf(node)) return;

    // check if leaf descendants are within the tolerance
    if (allLeavesWithinTolerance(node, node->avg, tolerance)) {
        // prune subtree
        clearHelper(node->NW);
        clearHelper(node->NE);
        clearHelper(node->SW);
        clearHelper(node->SE);
        // set children to nullptr after pruning
        node->NW = nullptr;
        node->NE = nullptr;
        node->SW = nullptr;
        node->SE = nullptr;
    } else {
        //  prune child subtrees
        pruneHelper(node->NW, tolerance);
        pruneHelper(node->NE, tolerance);
        pruneHelper(node->SW, tolerance);
        pruneHelper(node->SE, tolerance);
    }
}

bool QTree::allLeavesWithinTolerance(Node* node, const RGBAPixel& avg, double tolerance) {
    if (isLeaf(node)) {
        return node->avg.distanceTo(avg) <= tolerance;
    }

    //  check all children
    return (!node->NW || allLeavesWithinTolerance(node->NW, avg, tolerance)) &&
           (!node->NE || allLeavesWithinTolerance(node->NE, avg, tolerance)) &&
           (!node->SW || allLeavesWithinTolerance(node->SW, avg, tolerance)) &&
           (!node->SE || allLeavesWithinTolerance(node->SE, avg, tolerance));
}

bool QTree::isLeaf(Node* node) {
    return node && !node->NW && !node->NE && !node->SW && !node->SE;
}


