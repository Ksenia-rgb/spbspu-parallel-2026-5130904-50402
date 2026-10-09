#include "montecarlo.hpp"

#include <algorithm>
#include <istream>
#include <random>
#include <stdexcept>

std::istream &petrov::operator>>(std::istream &in, p_t &point)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  return in >> point.x >> point.y;
}

std::istream &petrov::operator>>(std::istream &in, circle_t &circle)
{
  std::istream::sentry sentry(in);
  if (!sentry)
  {
    return in;
  }
  double ignoredParameter = 0.0;
  return in >> circle.radius >> ignoredParameter >> circle.center;
}

std::vector< petrov::circle_t > petrov::readCircles(std::istream &in)
{
  std::vector< circle_t > circles;
  while (true)
  {
    in >> std::ws;
    if (in.eof())
    {
      break;
    }
    circle_t circle = {};
    if (!(in >> circle))
    {
      throw std::runtime_error("incorrect input data");
    }
    circles.push_back(circle);
  }
  return circles;
}

bool petrov::isInside(const p_t &point, const circle_t &circle)
{
  const double delX = point.x - circle.center.x;
  const double delY = point.y - circle.center.y;
  return ((delX * delX) + (delY * delY)) <= (circle.radius * circle.radius);
}

bool petrov::isInsideUnion(const p_t &point, const std::vector< circle_t > &circles)
{
  for (const circle_t &circle : circles)
  {
    if (isInside(point, circle))
    {
      return true;
    }
  }
  return false;
}

bool petrov::isInsideIntersection(const p_t &point, const std::vector< circle_t > &circles)
{
  for (const circle_t &circle : circles)
  {
    if (!isInside(point, circle))
    {
      return false;
    }
  }
  return true;
}

petrov::box_t petrov::findBoundingBox(const std::vector< circle_t > &circles)
{
  box_t box = { { 0.0, 0.0 }, { 0.0, 0.0 } };
  if (circles.empty())
  {
    return box;
  }
  const circle_t &firstCircle = circles.front();
  box.minPoint.x = firstCircle.center.x - firstCircle.radius;
  box.minPoint.y = firstCircle.center.y - firstCircle.radius;
  box.maxPoint.x = firstCircle.center.x + firstCircle.radius;
  box.maxPoint.y = firstCircle.center.y + firstCircle.radius;

  for (const circle_t &circle : circles)
  {
    box.minPoint.x = std::min(box.minPoint.x, circle.center.x - circle.radius);
    box.minPoint.y = std::min(box.minPoint.y, circle.center.y - circle.radius);
    box.maxPoint.x = std::max(box.maxPoint.x, circle.center.x + circle.radius);
    box.maxPoint.y = std::max(box.maxPoint.y, circle.center.y + circle.radius);
  }
  return box;
}

double petrov::computeBoxArea(const box_t &box)
{
  const double width = box.maxPoint.x - box.minPoint.x;
  const double height = box.maxPoint.y - box.minPoint.y;
  return width * height;
}

petrov::hits_t petrov::countHits(const std::vector< circle_t > &circles, const box_t &box,
    std::size_t tries, std::size_t seed)
{
  std::mt19937 generator(seed);
  std::uniform_real_distribution< double > xDistribution(box.minPoint.x, box.maxPoint.x);
  std::uniform_real_distribution< double > yDistribution(box.minPoint.y, box.maxPoint.y);

  hits_t hits = { 0, 0 };
  for (std::size_t attempt = 0; attempt < tries; ++attempt)
  {
    const double x = xDistribution(generator);
    const double y = yDistribution(generator);
    const p_t point = { x, y };
    if (isInsideUnion(point, circles))
    {
      ++hits.unionCount;
    }
    if (isInsideIntersection(point, circles))
    {
      ++hits.intersectionCount;
    }
  }
  return hits;
}

double petrov::computeArea(const box_t &box, std::size_t hits, std::size_t tries)
{
  const double hitShare = static_cast< double >(hits) / static_cast< double >(tries);
  return computeBoxArea(box) * hitShare;
}
