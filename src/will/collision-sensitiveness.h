// this will means that when another character occupies the same space, this character will react somehow

template <typename T>
bool overlaps_dimension(const T x1, const T w1, const T x2, const T w2) {
    if (x1 < x2) {
        return overlaps_dimension(x2, w2, x1, w1); // swapped
    }
    return (x2 - x1) < w1;
}

template <typename TRect>
bool overlaps(TRect const &r1, TRect const &r2)
{

    // overlaps on a rectangle, means it overlaps on both X and Y
    return overlaps_dimension(r1.x, r1.w, r2.x, r2.w) && overlaps_dimension(r1.y, r1.h, r2.y, r2.h );


    // returns false
    // +++++          +++++
    // +   +          +   +
    // +++++          +++++

    // returns true
    // +++++++
    // +  ++++++
    // +  +  + +
    // +  ++++++
    // +++++++

    // a rectangle lives in 2 dimensional space

    // we compare by using each of those dimensions

    // on each dimension, each rectangle looks like a segment


    // given the X dimension, r1 spans from r1.x by r1.w pixels, and r2 from r2.x by r2.w pixels
    // r1.x r1.w.. r2.x r2.w
    // how do we determine if two segments overlap?

    // returns false
    // r1: +++++                 (r1.x = 0, r1.w = 5)
    // r2:                +++++  (r2.x = 12, r2.w = 5)

    // returns true
    // r1: +++++++          (r1.x = 0, r1.w = 7)
    // r2:    ======        (r2.x = 4, r2.w = 6)

    // auto overlaps_on_x = (r1.x ... r1.w ... r2.x ... r2.w);

    // (r2.x - r1.x) < r1.w
    
    // CASE 1 returns false (unrelated)
    // r1: +++++                 (r1.x = 0, r1.w = 5)
    // r2:                +++++  (r2.x = 12, r2.w = 5)
    // (r1.x <= r2.x) && (r2.x <= (r1.x + r1.w))
    // (0 <= 12) && (12 <= (0 + 5))
    // true && (12 <= 5)
    // true && false
    // false

    // (r2.x - r1.x) < r1.w
    // (12 - 0) < 5

    // CASE 2 returns false (unrelated)
    // r1:                +++++  (r1.x = 12, r1.w = 5)
    // r2: +++++                 (r2.x = 0, r2.w = 5)
    // (r1.x <= r2.x) && (r2.x <= (r1.x + r1.w))
    // (12 <= 0) && (0 <= (12 + 5))
    // false && ?
    // false

    // VB.NET:
    // a AND b will evaluate a and also b, regardless of a
    // a AND ALSO b will evaluate a and if a is false, not continue

    // CASE 3 returns true (simple overlap, share some space)
    // r1: +++++++          (r1.x = 0, r1.w = 7)
    // r2:    ======        (r2.x = 4, r2.w = 6)

    // (r2.x - r1.x) < r1.w
    // (4 - 0) < 7
    // true

    // r2.x contained in r1.x .. r1.x + r1.w
    // how do you determine if a number is within a range?

    // is 4 within the range (0..7) ? 
    // is 4 within the range [0..7] ? 
    // (0 <= 4) && (4 <= 7)
    // replace 4 with r2.x, 0 with r1.x 7 (r1.x + r1.w)
    // (r1.x <= r2.x) && (r2.x <= (r1.x + r1.w))


    // CASE 4 returns true (simple overlap, share some space)
    // r1:    ======        (r1.x = 4, r1.w = 6)
    // r2: +++++++          (r2.x = 0, r2.w = 7)

    // (r1.x <= r2.x) && (r2.x <= (r1.x + r1.w))
    // (4<=0) &&
    // false

    // CASE 5 returns true (fully contained)
    // r1: +++++++          (r1.x = 0, r1.w = 7)
    // r2:    ==            (r2.x = 4, r2.w = 2)

    // (r1.x <= r2.x) && (r2.x <= (r1.x + r1.w))


    // (r2.x - r1.x) < r1.w
    // (4 - 0) < 7
    // true


    // CASE 6 returns true (fully contained)
    // r1:    ==            (r1.x = 4, r1.w = 2)
    // r2: +++++++          (r2.x = 0, r2.w = 7)

    // (r1.x <= r2.x) && (r2.x <= (r1.x + r1.w))
    

    // given the Y dimension, r1 spans from r1.y by r1.h pixels, and r2 from r2.y by r2.h pixels
    // r1.y r1.h.. r2.y r2.h
    // return true;
}

template <typename TCharacter, typename ActionT>
class CollisionSensitiveness
{
public:
    // you create a sensitiveness by defining the action that will happen
    CollisionSensitiveness(TCharacter &subject, ActionT reaction)
    {
        subject.addUpdate([&](auto *scene) {
            auto result = true;
            // find characters that are within the same space
            scene->foreach_character([subject, reaction, &result](auto const ch) {
                if ((&subject != ch.get()) && overlaps(subject._position, ch->_position))
                {
                    // when they share the space, we are sensitive so react
                    result &= reaction(ch.get());
                }
            });
            return result;
        });
    }
};
