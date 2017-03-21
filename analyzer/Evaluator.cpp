#include "Evaluator.h"

void sb::Evaluator::staticEvaluate( sb::Line& line ) const
{
	LineRating rating = line.getRating();

	const double length = line.getLength();
	const double angle = line.getAngleWithOx();
	const cv::Point2d& startingPoint = line.getStartingPoint();
	const cv::Point2d& endingPoint = line.getEndingPoint();

	if ( _direction == 0 ) {
		double normalizedAngle = MIN( angle, 180 - angle );

		// * xem xét tính toán sao cho độ dài không ảnh hưởng đến chỉ số đánh giá "là line" của các line ở phần trên khung hình
		// * chỉ số đánh giá chỉ ảnh hưởng bởi góc xoay và độ dài

		// vị trí dựa trên góc

		// góc gần alpha
		rating.rateToBeLane +=
				endingPoint.y * 0.1 *
				length * 0.3 *
				(90 - abs( normalizedAngle - _alpha )) * 0.4;


		// góc xoay ngang
		if ( normalizedAngle < ANGLE_CONSIDERED_AS_HORIZONTAL ) {
			rating.rateToBeLane -=
					endingPoint.y * 0.1 *
					length * 0.3 *
					(ANGLE_CONSIDERED_AS_HORIZONTAL - normalizedAngle) * 0.4;
		}

		// góc xoay đứng
		if ( normalizedAngle > ANGLE_CONSIDERED_AS_VERTICAL ) {
			rating.rateToBeUneven +=
					endingPoint.y * 0.1 *
					length * 0.1 *
					(normalizedAngle - ANGLE_CONSIDERED_AS_VERTICAL) * 0.4;
		}

	}

	line.setRating( rating );
}

float sb::Evaluator::normalize( float x, float min, float max )
{
	return (x - min) / (max - min);
}
