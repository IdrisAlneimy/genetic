#include "catch.hpp"

#include <algorithm>

#include "../src/population.hpp"
#include "../src/random.hpp"
#include "../src/sequence.hpp"

TEST_CASE( "Test for random", "[random]" ) {
   random::seed(1);

   SECTION ( "random float inside range" ) {
      float rand_f = random::uniform(0, 0.5);
      REQUIRE(rand_f < 0.5);
      REQUIRE(rand_f > 0.0);
   }

   SECTION ( "random integer inside positive range" ) {
      int rand_i = random::randint(0, 5);
      REQUIRE(rand_i <= 5);
      REQUIRE(rand_i >= 0);
   }

   SECTION ( "random integer inside negative range" ) {
      int rand_i = random::randint(-10, -5);
      REQUIRE(rand_i <= -5);
      REQUIRE(rand_i >= -10);
   }

   SECTION ( "random integer range with both" ) {
      int rand_neg = random::randint(-5, 5);
      REQUIRE(rand_neg <= 5);
      REQUIRE(rand_neg >= -5);
   }

   SECTION ( "repeat random integers not equal" ) {
      int rand_1 = random::randint(0, 100);
      int rand_2 = random::randint(0, 100);
      REQUIRE(rand_1 != rand_2);
   }

   SECTION ( "random reset produces repeatable results" ) {
      random::reset();
      int rand_1 = random::randint(0, 100);
      random::reset();
      int rand_2 = random::randint(0, 100);
      REQUIRE(rand_1 == rand_2);
   }

   SECTION ( "different seeds, different results" ) {
      random::seed(2);
      int rand_1 = random::randint(0, 100);
      random::seed(3);
      int rand_2 = random::randint(0, 100);
      REQUIRE(rand_1 != rand_2);
   }

   SECTION ( "random vector all within range" ) {
      random::seed(2);
      auto rand_set = random::sample(0, 100, 20);
      // Check the size
      REQUIRE(rand_set.size() == 20);
      // Check the range
      for (auto i : rand_set) {
         CHECK(i <= 100);
         CHECK(i >= 0);
      }
   }

   SECTION ( "random vector all unique" ) {
      random::reset();
      auto rand_set = random::sample(0, 100, 20, true);
      // Check the size
      REQUIRE(rand_set.size() == 20);
      // Check for uniqueness
      sort( rand_set.begin(), rand_set.end() );
      REQUIRE( adjacent_find(rand_set.begin(), rand_set.end() ) == rand_set.end() );
      // Check the range
      for (auto i : rand_set) {
         CHECK(i <= 100);
         CHECK(i >= 0);
      }
   }

   SECTION ( "random vector invalid range" ) {
      random::reset();
      REQUIRE_THROWS(random::sample(0, 5, 20, true));
   }
}

TEST_CASE( "Test for genetic::sequence", "[sequence]" ) {
   // Predictable random tests (that passed before)
   random::seed(1);
   SECTION ( "genes of two rand inits are different" ) {
      genetic::sequence a, b;
      // Two random individuals don't match
      REQUIRE_FALSE(a == b);
      REQUIRE_FALSE(a.get_fitness() == b.get_fitness());
   }

   SECTION ( "mutations with different rates" ) {
      // Copy constructor
      genetic::sequence a;
      genetic::sequence a_dup = a;
      REQUIRE(a_dup == a);
      // Mutate (0 = none)
      a.mutate(0.0);
      REQUIRE(a_dup == a);
      REQUIRE(a_dup.get_fitness() == a.get_fitness());
      // Mutation > 0.0 should modify genes
      a.mutate(0.5);
      REQUIRE_FALSE(a_dup == a);
      REQUIRE(a_dup.get_fitness() != a.get_fitness());
      // Mutation == 1.0 should modify again
      a.mutate(1.0);
      REQUIRE_FALSE(a_dup == a);
      REQUIRE(a_dup.get_fitness() != a.get_fitness());
   }

   SECTION ( "breed results in differnet children" ) {
      genetic::sequence a,b;
      // New child doesn't exactly match either parent
      genetic::sequence child = genetic::sequence::breed(a, b);
      REQUIRE_FALSE(child == a);
      REQUIRE_FALSE(child == b);

      // Siblings shouldn't match
      genetic::sequence sibling = genetic::sequence::breed(a, b);
      REQUIRE_FALSE(sibling == child);
      REQUIRE_FALSE(sibling == a);
   }
}

TEST_CASE( "Test for genetic::population", "[population]" ) {
   // Predictable random tests (that passed before)
   random::seed(1);

   SECTION ( "population size constant through evoluation, factor of 3" ) {
      genetic::population<genetic::sequence> pop(99);
      REQUIRE(pop.get_size() == 99);
      pop.evolve(1, genetic::sequence::get_max_fitness(), 0.2, true);
      REQUIRE(pop.get_size() == 99);
   }

   SECTION ( "population size constant through evoluation, non-factor of 3" ) {
      genetic::population<genetic::sequence> pop_odd(101);
      REQUIRE(pop_odd.get_size() == 101);
      pop_odd.evolve(1, genetic::sequence::get_max_fitness(), 0.2, true);
      REQUIRE(pop_odd.get_size() == 101);
   }

   SECTION ( "evoluation should improve fitness" ) {
      genetic::population<genetic::sequence> pop(99);
      auto pop_old = pop;
      pop.evolve(1, genetic::sequence::get_max_fitness(), 0.2, true);
      REQUIRE(pop.get_fittest().get_fitness() >= pop_old.get_fittest().get_fitness());
      REQUIRE(pop.get_fitness() >= pop_old.get_fitness());
   }

   SECTION ( "20 evoluation cycles" ) {
      genetic::population<genetic::sequence> pop(99);
      auto pop_old = pop;
      pop.evolve(20, genetic::sequence::get_max_fitness(), 0.1, true);
      REQUIRE(pop.get_fittest().get_fitness() >= pop_old.get_fittest().get_fitness());
      REQUIRE(pop.get_fitness() >= pop_old.get_fitness());
      REQUIRE(pop.get_size() == pop_old.get_size());
   }
}

TEST_CASE( "Test for previous results", "[results]" ) {
   // Predictable random tests (that passed before)
   random::seed(1);

   SECTION ( "'easy' solution" ) {
      genetic::sequence::set_solution("easy");
      genetic::population<genetic::sequence> pop(99);
      REQUIRE(pop.evolve(32, genetic::sequence::get_max_fitness(), 0.08, true));
   }
}