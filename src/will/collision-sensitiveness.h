// this will means that when another character occupies the same space, this character will react somehow

template <typename TRect>
bool overlaps(TRect const &r1, TRect const &r2)
{
    return true;
}

template <typename TCharacter, typename ActionT>
class CollisionSensitiveness
{
public:
    // you create a sensitiveness by defining the action that will happen
    CollisionSensitiveness(TCharacter subject, ActionT reaction)
    {
        subject.addUpdate([this](auto *scene) {
            // find characters that are within the same space
            scene.foreach_character([subject, reaction](TCharacter &ch) {
                if (overlaps(subject._position) n, ch._position)
                {
                    // when they share the space, we are sensitive so react
                    reation(ch);
                }
            });
            return true;
        });
    }
};
