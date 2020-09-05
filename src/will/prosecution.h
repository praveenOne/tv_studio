#include <functional>
#include <memory>
#include <map>

template <typename TCharacter, typename SceneType, typename TMove>
class Prosecution
{
private:
    TCharacter &_subject;
    TCharacter const &_target;
    bool _isEndOnCollision;
    std::shared_ptr<TMove> _currentMove;
    uint32_t _lastUpdate{0};
    std::map<HDirection, std::function<void()>> _setupDirections;

public:
    Prosecution(TCharacter &subject, const TCharacter &target, bool isEndOnCollision, std::map<HDirection, std::function<void()>> setupDirections)
    : _subject{subject}, _target{target}, _isEndOnCollision{isEndOnCollision}, _setupDirections{setupDirections} // initialization list, when we have reference we have to use it (references need to be initialize)
    {
        // "duck typing": "if it looks like a duck, sounds like a duck, walks like a duck, then it's a duck"
        _subject.addUpdate([this](auto *scene) { return UpdateSubject(scene); });
        // nowadays, we don't use "bind" but prefer a lambda like that before
        // _subject.addUpdate(std::bind(&Prosecution<TCharacter,SceneType>::UpdateSubject, this, std::placeholders::_1));
    }

    bool UpdateSubject(SceneType *scene)
    {
        // we need to follow the target, two options:
        // 1. we make the position of our subject closer to the target, or
        // 2. we create and maintain a HMove that will go towards the target

        // the character's level of dispersion will decide how fast we update the moves
        auto dispersionCount = _subject.dispersionCount();

        // when it's time to update a move, we will cancel the outstanding move and create a new one
        auto currentTime = scene->age();
        auto keep_will{true};
        if ((currentTime - _lastUpdate) > dispersionCount)
        {
            auto direction{_subject._position.x < _target._position.x ? HDirection::right : HDirection::left};
            if (_currentMove)
            {
                if (_currentMove->direction() != direction)
                {
                    _currentMove->cancel();
                    _currentMove.reset();
                    keep_will = !_isEndOnCollision;
                }
            }
            else
            {
                auto factory{_subject.moveFactory()};
                auto setupPos {_setupDirections.find(direction)};
                if (setupPos != _setupDirections.end()) {
                    setupPos->second();
                }
                _currentMove = factory(direction, currentTime);
            }
            _lastUpdate = currentTime;
        }
        return keep_will;
    }
};
