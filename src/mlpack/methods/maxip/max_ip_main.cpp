/**
 * @file max_ip_main.cpp
 * @author Ryan Curtin
 *
 * Main executable for maximum inner product search.
 */
#include <mlpack/core.hpp>
#include <mlpack/core/kernels/linear_kernel.hpp>

#include "max_ip.hpp"

using namespace std;
using namespace mlpack;
using namespace mlpack::maxip;
using namespace mlpack::kernel;

// Needs to be fleshed out a little bit.
PROGRAM_INFO("Maximum Inner Product Search",
    "This program will find the k maximum inner products of a set of points, "
    "using a query set and a reference set (which can optionally be the same "
    "set). More specifically, for each point in the query set, the k points in"
    " the reference set with maximum inner products are found.  The inner "
    "product used is the kernel function specified by --kernel.  Currently the"
    " linear kernel is all that is used.");

// Define our input parameters.
PARAM_STRING_REQ("reference_file", "File containing the reference dataset.",
    "r");
PARAM_STRING("query_file", "File containing the query dataset.", "q", "");

PARAM_INT_REQ("k", "Number of maximum inner products to find.", "k");

PARAM_STRING("products_file", "File to save inner products into.", "p", "");
PARAM_STRING("indices_file", "File to save indices of inner products into.",
    "i", "");

PARAM_FLAG("naive", "If true, O(n^2) naive mode is used for computation.", "N");
PARAM_FLAG("single", "If true, single-tree search is used (as opposed to "
    "dual-tree search.", "s");

int main(int argc, char** argv)
{
  CLI::ParseCommandLine(argc, argv);

  // Get reference dataset filename.
  const string referenceFile = CLI::GetParam<string>("reference_file");

  const size_t k = CLI::GetParam<int>("k");

  const bool naive = CLI::HasParam("naive");
  const bool single = CLI::HasParam("single");

  arma::mat referenceData;
  arma::mat queryData;

  data::Load(referenceFile, referenceData, true);

  Log::Info << "Loaded reference data from '" << referenceFile << "' ("
      << referenceData.n_rows << " x " << referenceData.n_cols << ")." << endl;

  // Sanity check on k value.
  if (k > referenceData.n_cols)
  {
    Log::Fatal << "Invalid k: " << k << "; must be greater than 0 and less ";
    Log::Fatal << "than or equal to the number of reference points (";
    Log::Fatal << referenceData.n_cols << ")." << endl;
  }

  // Load the query matrix, if we can.
  if (CLI::HasParam("query_file"))
  {
    const string queryFile = CLI::GetParam<string>("query_file");
    data::Load(queryFile, queryData, true);

    Log::Info << "Loaded query data from '" << queryFile << "' ("
        << queryData.n_rows << " x " << queryData.n_cols << ")." << endl;
  }
  else
  {
    Log::Info << "Using reference dataset as query dataset (--query_file not "
        << "specified)." << endl;
  }

  // Naive mode overrides single mode.
  if (naive && single)
  {
    Log::Warn << "--single ignored because --naive is present." << endl;
  }

  // Matrices for output storage.
  arma::Mat<size_t> indices;
  arma::mat products;

  // Construct MaxIP object.
  if (queryData.n_elem == 0)
  {
    MaxIP<LinearKernel> maxip(referenceData, (single && !naive), naive);
    maxip.Search(k, indices, products);
  }
  else
  {
    MaxIP<LinearKernel> maxip(referenceData, queryData, (single && !naive),
        naive);
    maxip.Search(k, indices, products);
  }

  // Save output, if we were asked to.
  if (CLI::HasParam("products_file"))
  {
    const string productsFile = CLI::GetParam<string>("products_file");
    data::Save(productsFile, products, false);
  }

  if (CLI::HasParam("indices_file"))
  {
    const string indicesFile = CLI::GetParam<string>("indices_file");
    data::Save(indicesFile, indices, false);
  }
}
