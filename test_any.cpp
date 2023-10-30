#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <vector>

#include "any.hpp"

TEST_CASE("Constructor") {
  SECTION("Default") {
    [[maybe_unused]] const Any any;
  }

  SECTION("Trivial types") {
    const Any any = 10;
    REQUIRE(AnyCast<int>(any) == 10);
    SECTION("Copy") {
      Any other_any = any;
      REQUIRE(AnyCast<int>(any) == 10);
      REQUIRE(AnyCast<int>(other_any) == 10);
    }
    SECTION("Move") {
      Any other_any(std::move(any));
      REQUIRE(AnyCast<int>(other_any) == 10);
    }
  }

  SECTION("Non-trivial types") {
    const Any any(std::vector<int>(10));
    REQUIRE(AnyCast<std::vector<int>>(any).size() == 10);
    SECTION("Copy") {
      Any other_any(any);
      REQUIRE(AnyCast<std::vector<int>>(any).size() == 10);
      REQUIRE(AnyCast<std::vector<int>>(other_any).size() == 10);
    }
    SECTION("Move") {
      Any other_any(std::move(any));
      REQUIRE(AnyCast<std::vector<int>>(other_any).size() == 10);
    }
  }
}

TEST_CASE("Assignment") {
  Any a = 10;
  Any b;

  SECTION("Value assignment") {
    a = 11;
    REQUIRE(AnyCast<int>(a) == 11);
  }

  SECTION("Copy") {  // copy assignment
    a = 10;
    b = a;
    REQUIRE(AnyCast<int>(b) == 10);
    SECTION("Copy is independent") {
      b = 1.0;
      REQUIRE(AnyCast<int>(a) == 10);
      REQUIRE(AnyCast<double>(b) == 1.0);
    }
  }

  SECTION("Move") {
    b = std::move(a);
    REQUIRE(AnyCast<int>(b) == 10);
  }

  SECTION("Self") {
    a = a;
    REQUIRE(AnyCast<int>(a) == 10);
  }
}

TEST_CASE("Swap") {
  Any a = 1;
  Any b = &a;

  a.Swap(b);
  REQUIRE(AnyCast<Any*>(a) == &a);
  REQUIRE(AnyCast<int>(b) == 1);

  b.Swap(b);
  REQUIRE(AnyCast<int>(b) == 1);
}

TEST_CASE("HasValue") {
  Any a;
  REQUIRE_FALSE(a.HasValue());

  Any b = 11;
  REQUIRE(b.HasValue());

  a = b;
  REQUIRE(a.HasValue());
  REQUIRE(b.HasValue());

  a = std::move(b);
  REQUIRE(a.HasValue());
  REQUIRE_FALSE(b.HasValue());

  a.Reset();
  REQUIRE_FALSE(a.HasValue());
}

TEST_CASE("Bad cast") {
  SECTION("Empty") {
    Any any;
    REQUIRE_THROWS_AS(AnyCast<int>(any), std::bad_cast);
  }

  SECTION("Invalid") {
    Any any = 11;
    struct EmptyTag {};
    REQUIRE_THROWS_AS(AnyCast<EmptyTag>(any), std::bad_cast);
  }
}