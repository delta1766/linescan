#include <linescan/collect_points.hpp>

#include <numeric>


namespace linescan{

	struct check_environement{
		check_environement(
			mitrax::raw_bitmap< bool > const& image,
			mitrax::raw_bitmap< bool >& in_use
		):
			image(image),
			in_use(in_use)
			{}

		void operator()(std::size_t x, std::size_t y){
			if(
				x >= in_use.cols() ||
				y >= in_use.rows() ||
				in_use(x, y) ||
				!image(x, y)
			) return;

			in_use(x, y) = true;

			points.emplace_back(x, y);

			(*this)(x - 1, y - 1);
			(*this)(x - 1, y);
			(*this)(x - 1, y + 1);
			(*this)(x, y - 1);
			(*this)(x, y + 1);
			(*this)(x + 1, y - 1);
			(*this)(x + 1, y);
			(*this)(x + 1, y + 1);
		};

		mitrax::raw_bitmap< bool > const& image;
		mitrax::raw_bitmap< bool >& in_use;

		vector< point< std::size_t > > points;
	};

	vector< std::pair< point< float >, std::size_t > >
	collect_points(mitrax::raw_bitmap< bool > const& image){
		auto in_use =
			mitrax::make_matrix< bool >(dims(image.cols(), image.rows()));

		vector< std::pair< point< float >, std::size_t > > result;

		for(std::size_t y = 0; y < image.rows(); ++y){
			for(std::size_t x = 0; x < image.cols(); ++x){
				if(in_use(x, y)) continue;

				if(!image(x, y)){
					in_use(x, y) = true;
					continue;
				}

				check_environement check(image, in_use);
				check(x, y);

				auto sum = std::accumulate(
					check.points.begin(),
					check.points.end(),
					point< std::size_t >()
				);

				result.emplace_back(point< float >(
					static_cast< float >(sum.x()) / check.points.size(),
					static_cast< float >(sum.y()) / check.points.size()
				), check.points.size());
			}
		}

		return result;
	}


}
